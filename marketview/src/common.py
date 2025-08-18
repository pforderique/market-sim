"""Common screen utils"""
import curses
import curses.ascii
import os
import enum

import npyscreen

from src import transaction


MARKET_RECORDER_API = os.getenv("MARKET_RECORDER_API", "http://localhost:5001")
print(f"Market Recorder API: {MARKET_RECORDER_API}")
SUPPORTED_SECURITIES = [s.name for s in transaction.SecurityID]
print(f"Supp: {SUPPORTED_SECURITIES}")


class Screen(str, enum.Enum):
    """Screen enumeration for different views."""
    HOME = "MAIN"
    SEARCH = "SEARCH"
    WATCHLIST = "WATCHLIST"
    STOCKCHART = "STOCKCHART"


def add_close_button(form: npyscreen.FormBaseNew):
    """Adds a close button to the form."""
    form.add(
        npyscreen.ButtonPress,
        name="X",
        when_pressed_function=lambda: form.parentApp.switchForm(None),
        relx=form.columns - 8,
        rely=1,
        color="DANGER",
    )


def add_ctrl_c_handler(form: npyscreen.FormBaseNew):
    """
    Adds a handler for Ctrl-C to exit the application.
    """
    form.add_handlers({
        curses.ascii.ETX: lambda _: form.parentApp.switchForm(None)
    })


def format_price(price):
    """Formats a price value as a string."""
    return f"${price:.2f}"
