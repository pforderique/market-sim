FROM python:3.11-slim

WORKDIR /app
COPY marketrecorder/requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY marketrecorder/ .
CMD ["python", "app.py"]
