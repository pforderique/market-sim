#!/usr/bin/env bash
set -euo pipefail

EXCHANGE_HOST="localhost"
EXCHANGE_PORT=8888
DELAY=15  # seconds

# Symbols
SYMBOLS="AMZN GOOG MSFT O AAPL TSLA"

# Medians
BID_AMZN=220;  ASK_AMZN=230;  DRIFT_AMZN=8
BID_GOOG=169;  ASK_GOOG=179;  DRIFT_GOOG=2
BID_MSFT=410;  ASK_MSFT=430;  DRIFT_MSFT=5
BID_AAPL=235;  ASK_AAPL=240;  DRIFT_AAPL=10
BID_TSLA=320;  ASK_TSLA=350;  DRIFT_TSLA=30
BID_O=54;      ASK_O=56;      DRIFT_O=1.5

# helpers for bash3 (indirect expansion)
get() { eval "echo \${$1}"; }
setvar() { eval "$1=\"$2\""; }

gen_price() {
  local median="$1" skew_dir="$2" width=16 a b
  if [ "$skew_dir" = "left" ]; then a=2; b=5; else a=5; b=2; fi
  python3 -c "import random; print(round(${median} + (${width}*(random.betavariate($a,$b)-0.5)), 2))"
}

send_order() {
  local side="$1" ticker="$2" price="$3"
  local order="${side}|${ticker}|${price}|1|simuser"
  printf '%s\n' "$order" | nc "$EXCHANGE_HOST" "$EXCHANGE_PORT"
  printf '%s\n' "$order"
}

while true; do
  for symbol in $SYMBOLS; do
    bid_var="BID_${symbol}"; ask_var="ASK_${symbol}"; drift_var="DRIFT_${symbol}"

    bid_median="$(get "$bid_var")"
    ask_median="$(get "$ask_var")"
    drift_scale="$(get "$drift_var")"

    # prices
    BID_PRICE="$(gen_price "$bid_median" right)"
    ASK_PRICE="$(gen_price "$ask_median" left)"

    send_order "BID" "$symbol" "$BID_PRICE"
    send_order "ASK" "$symbol" "$ASK_PRICE"

    # drift (independent per symbol)
    DRIFT="$(python3 -c "import random; print(round(random.uniform(-$drift_scale, $drift_scale), 2))")"
    setvar "$bid_var" "$(python3 -c "print(round($bid_median + $DRIFT, 2))")"
    setvar "$ask_var" "$(python3 -c "print(round($ask_median + $DRIFT, 2))")"
  done
  sleep "$DELAY"
done
