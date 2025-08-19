FROM python:3.13-slim

# better Unicode/ANSI in terminals
ENV PYTHONUNBUFFERED=1 \
    TERM=xterm-256color \
    LC_ALL=C.UTF-8 \
    LANG=C.UTF-8

WORKDIR /app
COPY marketview/requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY marketview/ .
CMD ["python", "main.py"]