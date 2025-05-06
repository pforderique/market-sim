import datetime
import os

import requests
import dotenv

from src import cache


BASE_URL = "https://morning-star.p.rapidapi.com"
_REQUEST_LIMIT_PER_SECOND = 5
_REQUESTS_MADE_IN_LAST_SECOND = 0


class MoringstarAPI:
    """RapidAPI client for Morningstar API."""

    CACHE_ENABLED = True
    _INSTANCE = None
    _CACHE: cache.Cache = None
    _CALLS_MADE = 0

    def __init__(self, api_key: str):
        self.api_key = api_key
        self.headers = {
            "x-rapidapi-key": api_key,
            "x-rapidapi-host": "morning-star.p.rapidapi.com"
        }
        self.session = requests.Session()
        self.session.headers.update(self.headers)

    @classmethod
    def get_instance(cls):
        dotenv.load_dotenv()
        if cls._INSTANCE is not None:
            return cls._INSTANCE

        api_key = os.getenv("MORNINGSTAR_API_KEY")
        cls._INSTANCE = cls(api_key)
        cls._CACHE = cache.Cache.load()
        return cls._INSTANCE

    def get_stock_data(self, ticker, force_update=False) -> dict | None:
        """Fetch stock data for a given ticker symbol.

        Args:
            ticker (str): The stock ticker symbol.

        Returns:
            dict: The stock data, with fields 'symbol', 'name', 'price', 
                  'fair_market_value', 'exchange', and 'country'.
            None: If the ticker is not found or if there is an error.
        """
        # Get the performance ID for the ticker
        route = "/market/v3/auto-complete"
        ticker_info = MoringstarAPI._CACHE.get(route, ticker)
        if force_update or not ticker_info:
            ticker_info = self.session.get(
                f"{BASE_URL}{route}", params={"q": ticker}
            ).json()
            MoringstarAPI._CALLS_MADE += 1
            if not ticker_info:
                MoringstarAPI._CACHE.set({}, route, ticker)
                return None
            ticker_info = ticker_info[0]
            ticker_info.update({"LAST_CACHED": datetime.datetime.now()})
            MoringstarAPI._CACHE.set(ticker_info, route, ticker)

        # Get fair market value data
        performance_id = ticker_info["PerformanceId"]
        route = "/stock/v2/get-price-fair-value"
        fair_value_data = MoringstarAPI._CACHE.get(route, performance_id)
        if force_update or fair_value_data is None:
            fair_value_data = self.session.get(
                f"{BASE_URL}{route}",
                params={"performanceId": performance_id},
            ).json()["chart"]["chartDatums"]["recent"]
            fair_value_data.update({"LAST_CACHED": datetime.datetime.now()})
            MoringstarAPI._CACHE.set(fair_value_data, route, performance_id)
            MoringstarAPI._CALLS_MADE += 1

        # Fetch latest price data
        route = "/stock/v2/get-mini-chart-realtime-data"
        price_data = MoringstarAPI._CACHE.get(route, performance_id)
        if force_update or price_data is None:
            price_data = self.session.get(
                f"{BASE_URL}{route}",
                params={"performanceId": performance_id},
            ).json()
            price_data.update({"LAST_CACHED": datetime.datetime.now()})
            MoringstarAPI._CACHE.set(price_data, route, performance_id)
            MoringstarAPI._CALLS_MADE += 1

        # Fetch star rating data
        route = "/stock/v2/get-security-info"
        ratings = MoringstarAPI._CACHE.get(route, performance_id)
        if force_update or ratings is None:
            ratings = self.session.get(
                f"{BASE_URL}{route}",
                params={"performanceId": performance_id},
            ).json()
            ratings.update({"LAST_CACHED": datetime.datetime.now()})
            MoringstarAPI._CACHE.set(ratings, route, performance_id)
            MoringstarAPI._CALLS_MADE += 1

        return {
            "name": ticker_info["Name"],
            "ticker": ticker_info["RegionAndTicker"].split(":")[-1],
            "starRating": ratings["starRating"],
            "lastPrice": price_data["lastPrice"],
            "dayChange": price_data["dayChange"],
            "dayChangePer": price_data["dayChangePer"],
            "latestFairValue": fair_value_data["latestFairValue"],
            "discount": (
                float(price_data["lastPrice"]) / float(fair_value_data["latestFairValue"])
            ) if fair_value_data["latestFairValue"] and price_data["lastPrice"] else None,
            "uncertainty": fair_value_data["uncertainty"],
            "fairValueDate": fair_value_data["fairValueDate"],
        }
