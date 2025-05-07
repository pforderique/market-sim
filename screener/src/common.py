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

STOCKS = [
    # ETFS
    # "VOO", "QQQM", "VTI", "VOOG", "SPY", "SCHD", "JEPI", "ETHUSD", "SOXX", "DXJ",
    # "QQQ", "ISCG",

    # All Stocks
    # "NVDA", "GOOG", "MSFT", "LLY", "UNH", "BLK", "AAPL", "COST", "META",
    # "TSLA", "GOOGL", "COKE", "O", "CMG", "BRK.B", "NFLX", "NOC", "LCID",
    # "SPGI", "MCK", "V", "TSCO", "CAVA", "EPD", "DELL", "PANW",
    # "UAL", "CRWD", "WM", "BA","HSY", "DAL", "BX",
    # "FSLR", "AMAT", "AMZN", "MCD", "WMT", "AVGO", "KO", "AXP", "ADI",
    # "AMD", "TGT", "HOOD", "UBER", "COR", "IQV",
    # "DIS", "MU", "RIVN", "QCOM", "NLY", "ZTS", "CRBP", "IONQ",

    # Pre-Selected
    # "FSLR", "TSLA", "RIVN", "AAPL", "CMG", "TGT", "EPD", "HOOD", "AMZN",
    # "MU", "AVGO", "DAL", "UAL", "AMD", "CRWD", "COKE", "UBER", "UNH",
    # "PANW",
    "ZTS", "MCK", "CAVA", "HOOD", "AMD", "LLY", "NVDA", "TSLA", "MU", "CRWD", 
    "BX", "AVGO", "AMAT", "ADI", "NLY", "DELL", "AXP", "UNH", "BLK", "SPGI",
    "VRTX", "V",
]