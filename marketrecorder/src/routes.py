import http

import flask

from src import db


_DB = db.TransactionDB(db.get_real_engine())
quotes_bp = flask.Blueprint('quotes', __name__)


@quotes_bp.route("", methods=['GET'])
def get_quotes():
    """
    Fetch the market price of one or more securities by checking the most recent transaction.

    Market prices are determined by the most recent transaction for each security.

    Query parameters:
        symbols (comma-separated): List of security symbols to fetch prices for.

    Returns:
        JSON response with the latest price and timestamp for each symbol.
        If a symbol has no transactions, it will return None for that symbol.
        Example:
        {
            "AAPL": {"price": 150.00, "timestamp": "2023-10-01T12:00:00Z"},
            "GOOGL": {"price": 2800.00, "timestamp": "2023-10-01T12:00:00Z"},
            "MSFT": null
        }
    """
    symbols = flask.request.args.get('symbols')
    if not symbols:
        return flask.jsonify({"error": "No symbols provided"}), http.HTTPStatus.BAD_REQUEST

    symbols_list = symbols.split(',')
    quotes = {}

    for symbol in symbols_list:
        transaction = _DB.most_recent_transaction(symbol)
        if transaction:
            quotes[symbol] = {
                "price": transaction.price,
                "timestamp": transaction.timestamp,
            }
        else:
            quotes[symbol] = None

    return flask.jsonify(quotes), http.HTTPStatus.OK

@quotes_bp.route("<symbol>", methods=['GET'])
def get_quote(symbol):
    """
    Fetch the market price of a single security by checking the most recent transaction.

    Market prices are determined by the most recent transaction for the security.

    Returns:
        JSON response with the latest price and timestamp for the symbol.
        If the symbol has no transactions, it will return None.
        Example:
        {
            "price": 150.00,
            "timestamp": "2023-10-01T12:00:00Z"
        }
    """
    transaction = _DB.most_recent_transaction(symbol)
    if transaction:
        quote = {
            "price": transaction.price,
            "timestamp": transaction.timestamp,
        }
    else:
        quote = {}

    return flask.jsonify(quote), http.HTTPStatus.OK

@quotes_bp.route("<symbol>/history", methods=['GET'])
def get_quote_history(symbol):
    """
    Fetch the historical transactions for a given security symbol.

    Historical transactions are aggregated by time step (e.g., minute, hour, day).

    Query parameters:
        step (optional): The time step to aggregate by. One of 'min', 'hr', 'day', 'month'.
                         Default is 'day'.

    Returns:
        JSON response with a list of dictionaries containing period, avg_price,
        min_price, max_price, total_volume, and num_transactions.
        Example:
        [
            {
                "period": "2023-10-01T12:00:00Z",
                "avg_price": 150.00,
                "min_price": 148.00,
                "max_price": 152.00,
                "total_volume": 1000,
                "num_transactions": 10
            },
            ...
        ]
    """
    step = flask.request.args.get('step', 'day')
    transactions = _DB.historical_prices(symbol, step)
    return flask.jsonify(transactions), http.HTTPStatus.OK
