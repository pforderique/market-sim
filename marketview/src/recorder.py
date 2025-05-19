from collections import defaultdict
import socket
import threading

from src import transaction


class TransactionListener:
    def __init__(self, multicast_group="239.255.42.99", port=9999):
        self.multicast_group = multicast_group
        self.port = port
        self.running = False
        self.latest_prices = defaultdict(float)
        self.call_backs = []

    def _listen(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind(("", self.port))
        mreq = socket.inet_aton(self.multicast_group) + socket.inet_aton("0.0.0.0")
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

        while self.running:
            try:
                data, _ = sock.recvfrom(4096)
                txn = transaction.parse_transaction(data.decode())
            except Exception as e:
                print(f"Exception in TransactionListener: {e}")
                continue
            else:
                # Notify all listeners
                for cb in self.call_backs:
                    cb(txn)

    def start_background_listener(self):
        self.running = True
        threading.Thread(target=self._listen, daemon=True).start()

    def stop(self):
        self.running = False

    def register_callback(self, callback):
        self.call_backs.append(callback)