# Market Simulator

Open source implementation of a market exchange (exchange/), transaction consumer (marketrecorder/), and [PENDING] trader (trader/).

Also contains side tool (screener/) to integrate with Exchange in the future.

## Start the simulation

1) Start the [Exchange Server](exchange/README.md)
2) Start the [Market Recorder](marketrecorder/README.md) transaction listener
3) **[COMING SOON]** Start the visualization tool
4) **[COMING SOON]** Create or run the default bot traders


While (3) and (4) are being implemented, try these commands in the meantime:

`echo "BID|GOOG|165.00|1|fab" | nc localhost 8888`</br>
`echo "ASK|GOOG|165.50|1|luz" | nc localhost 8888`</br>
*(no transaction recorded)*</br>
`echo "BID|GOOG|166.00|1|andre" | nc localhost 8888`</br>
*(transaction recorded for 1 GOOG, luz &#8594; andre, $165.50)*</br>
`echo "ASK|GOOG|164.00|0.5|jorge" | nc localhost 8888`</br>
*(transaction recorded for 0.5 GOOG, jorge &#8594; fab, $165.00)*</br>

