# src/screens/home.py
import npyscreen

from src import common

class HomeScreen(npyscreen.FormBaseNew):
    def create(self):
        common.add_close_button(self)
        self.add(npyscreen.TitleText, name="Market View", editable=False)
        self.add(npyscreen.FixedText, value="Welcome to MarketView")
        self.add(npyscreen.FixedText, value="Press 's' to search for a stock")

    def handle_input(self, key):
        if key == ord("s"):
            # print("Search key pressed")
            self.parentApp.switchForm(common.Screen.SEARCH)
        else:
            super().handle_input(key)
