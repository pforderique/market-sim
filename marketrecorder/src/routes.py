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
        Example:
        {
            "AAPL": {"price": 150.00, "timestamp": "2023-10-01T12:00:00Z"},
            "GOOGL": {"price": 2800.00, "timestamp": "2023-10-01T12:00:00Z"}
        }
    """
    symbols = flask.request.args.get('symbols')
    if not symbols:
        return flask.jsonify({"error": "No symbols provided"}), 400

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
            quotes[symbol] = {"error": "No recent transaction found"}

    return flask.jsonify(quotes)
