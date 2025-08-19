from flask import Flask
import threading
import signal
import sys

from src import recorder
from src import routes

# Flask app setup
app: Flask = Flask(__name__)
app.register_blueprint(routes.quotes_bp, url_prefix="/quotes")

@app.route('/')
def home():
    return "Market Recorder is running!\n"

# Background thread for recorder
transaction_recorder = recorder.MarketRecorder()
recorder_thread = threading.Thread(
    target=transaction_recorder.listen,
    daemon=True,
)

# Graceful shutdown
def shutdown_handler(signal, frame):
    print("Shutting down...")
    transaction_recorder.stop()
    sys.exit(0)

signal.signal(signal.SIGINT, shutdown_handler)
signal.signal(signal.SIGTERM, shutdown_handler)

if __name__ == '__main__':
    recorder_thread.start()
    app.run(host="0.0.0.0", port=5000)
