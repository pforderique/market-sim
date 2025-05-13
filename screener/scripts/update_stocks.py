from src import morningstar as ms
from src import common
import time


if __name__ == "__main__":
    ms_api = ms.MoringstarAPI.get_instance()
    speeds = []
    for stock in common.STOCKS:
        start_time = time.time()
        data = ms_api.get_stock_data(stock, force_update=True)
        elapsed_time = time.time() - start_time

        speeds.append(elapsed_time)
        print(f"({elapsed_time:.2f}s) {data}")

    print(f"Total stocks: {len(common.STOCKS)}")
    print(f"Total time: {sum(speeds):.2f}s")
    print(f"Average speed: {sum(speeds) / len(speeds):.2f}s")
    print(f"Max speed: {max(speeds):.2f}s")
    print(f"Min speed: {min(speeds):.2f}s")
