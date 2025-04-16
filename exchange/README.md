# Exchange

Open source implementation of a market exchange.

## Run the Exchange Server

Build all the binaries
```bash
make all
```

Run the server
```bash
make run
```

In another terminal, send a bid request
```bash
echo "BID|MSFT|390.05|3|fab" | nc localhost 8888
```

And then an ask order
```bash
echo "ASK|MSFT|390.00|2|piero" | nc localhost 8888
```

The exchange server terminal will log (and eventually UDP broadcast) a transaction for 2 shares at $390.00. 

**Note:** the most recent order is favored to encourage market participants to submit their most "aggressive" orders.


Running with Docker is still under development.
Please ignore dockerfiles for now.