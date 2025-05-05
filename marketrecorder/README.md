# MarketRecorder

System responsible for logging transactions multicasted from the Exchange Server.
Stores transactions and maintains other tables for historical prices, volume, etc.
Provides a simple flask api to fetch historical prices.

## Run MarketRecorder

Connect to a virtual environment and install deps in `requirements.txt`.
Then run:

```bash
python app.py
```

This listen on port 9999 for multicast packets and serve the flask app.


Running with Docker is still under development.
Please ignore dockerfiles for now.