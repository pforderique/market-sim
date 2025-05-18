# src/recorder.py
import socket
import threading
from collections import defaultdict

class TransactionListener:
    def __init__(self, multicast_group="239.255.42.99", port=9999):
        self.multicast_group = multicast_group
        self.port = port
        self.running = False
        self.latest_prices = defaultdict(float)

    def _listen(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind(("", self.port))
        mreq = socket.inet_aton(self.multicast_group) + socket.inet_aton("0.0.0.0")
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

        while self.running:
            try:
                data, _ = sock.recvfrom(4096)
                txn = data.decode().strip().split("|")
                if len(txn) >= 3:
                    symbol = txn[0]
                    price = float(txn[1])
                    self.latest_prices[symbol] = price
            except Exception:
                continue

    def start_background_listener(self):
        self.running = True
        threading.Thread(target=self._listen, daemon=True).start()

    def stop(self):
        self.running = False
