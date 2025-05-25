#!/bin/bash

# Configuration
EXCHANGE_HOST="localhost"
EXCHANGE_PORT=8888
DELAY=60  # seconds

# Initial medians for each symbol
declare -A BID_MEDIANS=( ["AMZN"]=220 ["GOOG"]=169 ["MSFT"]=410 ["O"]=54 )
declare -A ASK_MEDIANS=( ["AMZN"]=230 ["GOOG"]=179 ["MSFT"]=430 ["O"]=56 )

# Drift scale per symbol (controls how fast each moves)
declare -A DRIFT_SCALE=( ["AMZN"]=8 ["GOOG"]=2 ["MSFT"]=5 ["O"]=1.5 )

# Use skewed Beta distribution via Python to simulate skew
gen_price() {
    local median=$1
    local skew_dir=$2  # "left" or "right"
    local width=16

    # Skew: a > b for left skew, a < b for right
    local a b
    if [ "$skew_dir" == "left" ]; then
        a=2; b=5
    else
        a=5; b=2
    fi

    python3 -c "import random; print(round(${median} + (${width} * (random.betavariate($a, $b) - 0.5)), 2))"
}

# Send 1-share order
send_order() {
    local side=$1
    local ticker=$2
    local price=$3
    local order="$side|$ticker|$price|1|simuser"
    echo $order | nc $EXCHANGE_HOST $EXCHANGE_PORT
    echo $order
}

# Main loop
while true; do
    for symbol in "${!BID_MEDIANS[@]}"; do
        # Generate skewed bid and ask prices
        BID_PRICE=$(gen_price ${BID_MEDIANS[$symbol]} "right")
        ASK_PRICE=$(gen_price ${ASK_MEDIANS[$symbol]} "left")

        send_order "BID" "$symbol" "$BID_PRICE"
        send_order "ASK" "$symbol" "$ASK_PRICE"

        # Apply symbol-specific drift
        DRIFT=$(python3 -c "import random; print(round(random.uniform(-${DRIFT_SCALE[$symbol]}, ${DRIFT_SCALE[$symbol]}), 2))")
        BID_MEDIANS[$symbol]=$(python3 -c "print(round(${BID_MEDIANS[$symbol]} + $DRIFT, 2))")
        ASK_MEDIANS[$symbol]=$(python3 -c "print(round(${ASK_MEDIANS[$symbol]} + $DRIFT, 2))")
    done

    sleep $DELAY
done
