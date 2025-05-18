import npyscreen
import requests

from src.common import MARKET_RECORDER_API, format_price


_NA = "N/A"

class StockChartScreen(npyscreen.FormBaseNew):
    def create(self):
        self.symbol = None
        self.name_widget = self.add(npyscreen.FixedText, value="", rely=2, relx=2)
        self.price_widget = self.add(npyscreen.FixedText, value="", rely=4, relx=2)
        self.chart_widget = self.add(npyscreen.BoxTitle, name="Price Chart", values=[], max_height=10, rely=6, relx=2)
        self.back_button = self.add(npyscreen.ButtonPress, name="Back", rely=-3, relx=2)
        self.back_button.whenPressed = self.on_back

    def set_symbol(self, symbol):
        self.symbol = symbol
        self.update_content()

    def update_content(self):
        if not self.symbol:
            return
        self.name_widget.value = f"{self.symbol} - {self.get_company_name()}"
        self.price_widget.value = f"Current Price: {self.get_current_price()}"
        self.chart_widget.values = self.get_price_history()
        self.display()

    def get_company_name(self):
        # Placeholder logic
        return "Placeholder Inc."

    def get_current_price(self):
        try:
            resp = requests.get(f"{MARKET_RECORDER_API}/quotes/{self.symbol}")
            return format_price(resp.json().get("price", 0))
        except:
            return _NA

    def get_price_history(self):
        try:
            resp = requests.get(f"{MARKET_RECORDER_API}/quotes/{self.symbol}/history?step=min")
            data = resp.json()
            print(f"Price history data: {data}")
            return [f"{t} - {format_price(p)}" for t, p in data]
        except:
            return ["No data"]

    def on_back(self):
        self.parentApp.setNextForm("SEARCH")
        self.parentApp.switchFormNow()
