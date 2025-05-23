import enum

import npyscreen

from src import transaction


MARKET_RECORDER_API = "http://localhost:5000"

SUPPORTED_SECURITIES = [s.name for s in transaction.SecurityID]
print(f"Supp: {SUPPORTED_SECURITIES}")

class Screen(str, enum.Enum):
    HOME = "MAIN"
    SEARCH = "SEARCH"
    WATCHLIST = "WATCHLIST"
    STOCKCHART = "STOCKCHART"

def add_close_button(form: npyscreen.FormBaseNew):
    form.add(
        npyscreen.ButtonPress,
        name="X",
        when_pressed_function=lambda: form.parentApp.switchForm(None),
        relx=form.columns - 8,
        rely=1,
        color="DANGER",
    )

def format_price(price):
    return f"${price:.2f}"
