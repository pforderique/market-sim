import npyscreen
import enum

class Screen(enum.Enum):
    MAIN = "MAIN"
    STOCK_SCREENER = "STOCK_SCREENER"


def add_close_button(form: npyscreen.FormBaseNew):
    form.add(
        npyscreen.ButtonPress,
        name="X",
        when_pressed_function=lambda: form.parentApp.switchForm(None),
        relx=form.columns - 8,
        rely=1,
        color="DANGER",
    )
