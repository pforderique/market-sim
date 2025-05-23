import enum
import datetime

import pydantic


class SecurityID(str, enum.Enum):
    """Enum for security IDs."""
    AAPL = "AAPL"
    MSFT = "MSFT"
    GOOG = "GOOG"
    AMZN = "AMZN"
    TSLA = "TSLA"
    O    = "O"


class Transaction(pydantic.BaseModel):
    """Model for a transaction."""
    # id: uuid.UUID 
    security_id: SecurityID
    price: float
    quantity: float
    buyer: str
    seller: str
    timestamp: datetime.datetime


def parse_transaction(data: str) -> Transaction:
    """Parse a transaction from a string.

    This function assumes the data is in the same format as the Exchange
    multicasts, i.e. "security|price|shares|buyer|seller|timestamp".

    Args:
        data (str): The transaction data as a string.

    Returns:
        Transaction: A Transaction object.

    Raises:
        ValueError: If the data is not in the expected format.
        ValidationError: If the data does not match the Transaction model.
    """
    parts = data.strip().split("|")
    if len(parts) != 6:
        raise ValueError("Invalid transaction format")

    keys = ("security_id", "price", "quantity", "buyer", "seller", "timestamp")
    mapping = dict(zip(keys, parts))

    return Transaction.model_validate(mapping)
