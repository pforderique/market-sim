import npyscreen
import dotenv

from src.screens.home import HomeScreen
from src.screens.stock_screener import StockScreener
import src.common as common


class PortfolioApp(npyscreen.NPSAppManaged):
    def onStart(self):
        self.addForm(common.Screen.MAIN.name, HomeScreen)
        self.addForm(common.Screen.STOCK_SCREENER.name, StockScreener)


if __name__ == "__main__":
    dotenv.load_dotenv()
    app = PortfolioApp()
    app.run()
