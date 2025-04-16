# exchange/Dockerfile
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    g++ \
    make \
    net-tools \
    iputils-ping \
    libgtest-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /exchange

COPY . .

RUN make server

CMD ["./build/main.exe"]
