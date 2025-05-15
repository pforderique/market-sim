import logging
import socket
import struct

from src import models
from src import db


MCAST_GRP = '239.255.42.99'  # must match the exchange broadcast address
PORT = 9999

logging.basicConfig(
    level=logging.DEBUG,
    format="%(asctime)s [%(levelname)s] %(name)s: %(message)s",
)
logger = logging.getLogger(__name__)


class MarketRecorder:
    def __init__(self):
        self.running = False
        self.transaction_db = db.TransactionDB()
        self.sock = socket.socket(
            socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

        try:
            # Some systems require this for reuse to work correctly
            self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
        except AttributeError:
            pass  # Not supported everywhere

        self.sock.bind(('', PORT))  # Bind to all interfaces on the given port

        # Join multicast group on all interfaces
        mreq = struct.pack("=4sl", socket.inet_aton(
            MCAST_GRP), socket.INADDR_ANY)
        self.sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

    def listen(self):
        logger.info(f"Listening on multicast group {MCAST_GRP}:{PORT}")
        self.running = True
        while self.running:
            try:
                data, _ = self.sock.recvfrom(4096)
                self.handle_message(data.decode())
            except Exception as e:
                logger.error("Error receiving message: %s", e)

    def stop(self):
        self.running = False

    def handle_message(self, data: str) -> None:
        """
        Handle incoming messages.

        Args:
            data (str): The incoming message data.
        """
        transaction = models.parse_transaction(data)
        logger.info("RECEIVED: \"%s\" | PARSED %s", data, transaction)
        self.transaction_db.insert_transaction(transaction)
