import npyscreen

from src import common
from src.screens import home
from src.screens import search
from src.screens import stockchart
from src import recorder


class MarketViewApp(npyscreen.NPSAppManaged):
    def onStart(self):
        self.addForm(common.Screen.HOME, home.HomeScreen)
        self.addForm(common.Screen.SEARCH, search.SearchScreen)
        self.addForm(common.Screen.STOCKCHART, stockchart.StockChartScreen)
        self.txn_listener = recorder.TransactionListener()
        self.txn_listener.start_background_listener()

    def onCleanExit(self):
        self.txn_listener.stop()


if __name__ == "__main__":
    app = MarketViewApp()
    app.run()
