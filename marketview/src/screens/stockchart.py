import curses
import npyscreen
import os

from src import common
from src.screens import chart


class StockChartScreen(npyscreen.FormBaseNew):
    def create(self):
        self.symbol = None
        self.uh_oh_widget = self.add(npyscreen.FixedText, value="Uh-oh! No stock was passed.", rely=2, relx=2)
        self.back_button = self.add(npyscreen.ButtonPress, name="Back", rely=-3, relx=2)
        self.back_button.whenPressed = self.on_back

    def set_symbol(self, symbol):
        self.symbol = symbol
        chart_screen = chart.ChartScreen(self.symbol)
        cb = chart_screen.on_transaction
        self.parentApp.txn_listener.register_callback(cb)

        curses.endwin()
        os.system("clear")
        chart_screen.show()
        input("Press [Enter] to return...")
        self.parentApp.txn_listener.remove_callback(cb)
        self.parentApp.run()

    def on_back(self):
        self.parentApp.switchForm(common.Screen.SEARCH)
