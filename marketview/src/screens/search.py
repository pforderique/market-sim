import npyscreen
from src import common
from src.common import SUPPORTED_SECURITIES

_SEARCH_HEADER = "Search Ticker:"
_CANCEL = "Cancel"

class SearchScreen(npyscreen.FormBaseNew):
    def create(self):
        y, x = self.useable_space()
        x_pad = 2
        self.add(npyscreen.TitleText, name=_SEARCH_HEADER, rely=2, relx=x_pad, editable=False)
        self.search_query = self.add(npyscreen.Textfield, rely=2, relx=x_pad + len(_SEARCH_HEADER) + 1, width=20)
        self.result_list = self.add(npyscreen.SelectOne, max_height=10, values=[], scroll_exit=True, rely=4, relx=x_pad)
        self.cancel_button = self.add(npyscreen.ButtonPress, name=_CANCEL, rely=y-3, relx=x_pad)
        self.ok_button = self.add(npyscreen.ButtonPress, name="OK", rely=y-3, relx=x_pad + len(_CANCEL) + 4)

        self.result_list.when_value_edited = self.show_ok
        self.search_query.when_value_edited = self.update_results
        self.cancel_button.whenPressed = self.on_cancel
        self.ok_button.whenPressed = self.on_ok
        self.ok_button.hidden = True

    def show_ok(self):
        # Show OK button only if something is selected
        self.ok_button.hidden = len(self.result_list.value) == 0
        self.ok_button.update(clear=False)

    def update_results(self):
        query = self.search_query.value.upper()
        if query:
            filtered = [s for s in SUPPORTED_SECURITIES if query in s]
            self.result_list.values = filtered
        else:
            self.result_list.values = []
        self.result_list.display()

        # If no results, and thus nothing selected, hide the OK button
        if not self.result_list.values:
            self.ok_button.hidden = True
            self.ok_button.update(clear=False)
        elif self.result_list.values and self.result_list.value:
            # If there are results, show the OK button
            self.ok_button.hidden = False
            self.ok_button.update(clear=False)

    def on_ok(self):
        if self.result_list.value:
            selected_symbol = self.result_list.values[self.result_list.value[0]]
            self.parentApp.getForm(common.Screen.STOCKCHART).set_symbol(selected_symbol)
            self.parentApp.switchForm(common.Screen.STOCKCHART)

    def on_cancel(self):
        self.parentApp.setNextForm("MAIN")
        self.parentApp.switchFormNow()
