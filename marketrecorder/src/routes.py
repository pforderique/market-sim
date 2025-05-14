import flask


quotes_bp = flask.Blueprint('routes', __name__)

@quotes_bp.route('/quotes', methods=['GET'])
def get_quotes():
    """
    Fetch the market price of one or more securities by checking the most recent transaction.

    Query parameters:
        symbols (comma-separated): List of security symbols to fetch prices for.
    """
    symbols = flask.request.args.get('symbols')
    if not symbols:
        return flask.jsonify({"error": "No symbols provided"}), 400

    symbols_list = symbols.split(',')
    quotes = {}

    for symbol in symbols_list:
        transaction = get_most_recent_transaction(symbol)
        if transaction:
            quotes[symbol] = {
                "price": transaction['price'],
                "timestamp": transaction['timestamp'],
            }
        else:
            quotes[symbol] = {"error": "No recent transaction found"}

    return flask.jsonify(quotes)

