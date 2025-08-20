# Market Simulator

Open source implementation of a market exchange (exchange/), transaction consumer (marketrecorder/), and market view platform (marketview/).

* The **exchange** includes a thread-safe matching engine focused on recording ASK and BID orders via TCP with low latency. It broadcasts completed (or partial) order matchings via UDP. In real life, this could be something like the New York Stock Exchange (NYSE). 

* The **marketrecorder** listens for broadcasted transactions and records those in a database. It provides a simplified HTTP API for consuming market price and historical prices of securities. In real life, this would mimic a trading company, investment bank, or brokerage like Bear Stearns.

* The **marketview** application provides a simple terminal UI for viewing historical and realtime updates of these trades. It calls  **marketrecorder**'s API routes for historical prices and listens for the **exchange**'s UDP multicasts. In real life, this would be owned by a trading company or an individual investor.

## 0) Software Dependencies
Just need to make sure you have [docker](https://docs.docker.com/engine/install/) installed and that the docker daemon is running (can do this by opening up Docker Desktop).

## 1) Start all Services

The UI depends on the core infrastructure (marketrecorder and exchange services) to be running. You can either start them separately if you want to see the service logs as they log transactions, or you can start the core infrastructure in the background.

### A) Run in background

```bash
make view
```
Press 's' to bring up the search page and search for "GOOG". You'll notice it shows `[No Transactions Made Yet]`.

OR 

### B) Run with logs
```
make up
```
then in another terminal,
```
make view
```
Press 's' to bring up the search page and search for "GOOG". You'll notice it shows `[No Transactions Made Yet]`.

## 2) Place Orders
While the UI is pulled up on the "GOOG" page, run these commands in a separate terminal to simulate orders:

`echo "BID|GOOG|165.00|1|fab" | nc localhost 8888`</br>
`echo "ASK|GOOG|165.50|1|luz" | nc localhost 8888`</br>
*(no transaction recorded)*</br>
`echo "BID|GOOG|166.00|1|andre" | nc localhost 8888`</br>
*(transaction recorded for 1 GOOG, luz &#8594; andre, $165.50)*</br>
*(UI updated, no chart yet)*</br>
`echo "ASK|GOOG|164.00|0.5|jorge" | nc localhost 8888`</br>
*(transaction recorded for 0.5 GOOG, jorge &#8594; fab, $165.00)*</br>
*(UI updated, chart shows up)*</br>

## 3) Simulate the Market
Feel free to stay on the "GOOG" page or hit the `ENTER` key on the UI to go back to the search page and try another stock. Then, in a separate terminal, run a script that simulates a bunch of orders every 15 seconds to mock a (very low volume) market.

```
make orders
```
*(this just runs `sim_orders.sh`, so feel free to change that script's order frequency)*</br>

> Note, the UI notes new orders on every transaction, but will only update the chart once per minute. This is a known bug, not a feature ;)

**Congrats!** You just ran an end-to-end example of how our markets work.

To turn down all services, simply run `make down`.
