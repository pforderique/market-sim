FROM ubuntu:22.04

# Install build tools and SQLite
RUN apt-get update && apt-get install -y \
    g++ make cmake libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY exchange/ .
RUN mkdir -p data
# Build exchange server
RUN make server

CMD ["./build/main.exe"]
