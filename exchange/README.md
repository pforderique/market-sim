# Exchange

Open source implementation of a market exchange.

## Run the Exchange Server

Install dependencies
```bash
bash setup.sh
```

Build and run the server
```bash
make server && make run
```

In another terminal, send a bid request
```bash
echo "BID|MSFT|390.05|3|fab" | nc localhost 8888
```

And then an ask order
```bash
echo "ASK|MSFT|390.00|2|piero" | nc localhost 8888
```

The exchange server terminal will log and UDP multicast a transaction for 2 shares at $390.05. The multicasted transaction will look like this
`MSFT|390.05|2.00|fab|piero|2025-05-04T21:13:31` in the format `security|price|shares|buyer|seller|timestamp`, where datetime is in [iso 8601](https://www.iso.org/iso-8601-date-and-time-format.html) format.

**Note:** the most recent order is favored to encourage market participants to submit their most "aggressive" orders. Since the ASK order is the most recent, the request is fulfilled at the higher price, in the seller's favor.


Running with Docker is still under development.
Please ignore dockerfiles for now.

## Development

After making changes, build all main and test targets
```bash
make all
```

and run the tests
```bash
make test
```
