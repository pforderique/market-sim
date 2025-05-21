import os
import shutil

import plotext as plt
import requests

from src.common import MARKET_RECORDER_API, format_price
from src import transaction


_NA_PRICE = "[No Transactions Made Yet]"


def clear_terminal():
    os.system("cls" if os.name == "nt" else "clear")

class ChartScreen:
    def __init__(self, symbol):
        self.symbol = symbol

        price_data = requests.get(f"{MARKET_RECORDER_API}/quotes/{self.symbol}").json()
        if price_data:
            self.market_price = price_data.get("price", _NA_PRICE)
        else:
            self.market_price = _NA_PRICE

    def show(self):
        self.refresh_chart()

    def _get_plot_data(self):
        data = requests.get(f"{MARKET_RECORDER_API}/quotes/{self.symbol}/history?step=min").json()
        timestamp_labels = [p["timestamp"] for p in data]
        prices = [p["avg_price"] for p in data]
        return timestamp_labels, prices

    def on_transaction(self, txn: transaction.Transaction):
        if txn.security_id == self.symbol:
            self.market_price = txn.price
            self.refresh_chart()

    def refresh_chart(self):
        clear_terminal()

        timestamp_labels, prices = self._get_plot_data()
        plt.clear_figure()

        if self.market_price == _NA_PRICE:
            title = f"{self.symbol} {_NA_PRICE}"
        elif not prices:
            title = f"{self.symbol} {format_price(self.market_price)} [Chart not updated.]"
        else:
            title = f"{self.symbol} {format_price(self.market_price)}"
            plt.plot(range(len(timestamp_labels)), prices)
            plt.xticks(range(len(timestamp_labels)), timestamp_labels)
            plt.yticks(prices, [format_price(p) for p in prices])

        plt.title(title)
        plt.xlabel("Date")
        plt.ylabel("Price")

        cols, rows = shutil.get_terminal_size()
        plt.plotsize(cols, rows)
        plt.canvas_color("black")
        plt.axes_color("black")
        plt.ticks_color("white")
        # plt.grid(True, False)

        plt.show()
