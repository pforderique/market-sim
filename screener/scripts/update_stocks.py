from src import morningstar as ms
from src import common


if __name__ == "__main__":
    ms_api = ms.MoringstarAPI.get_instance()
    for stock in common.STOCKS:
        data = ms_api.get_stock_data(stock, force_update=False)
        print(f"{stock}".center(10))
        print(data)
