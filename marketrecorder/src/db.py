import sqlmodel
import pathlib
from src import models

DB_FILE = "data/transactions.db"
DB_URL = f"sqlite:///{DB_FILE}"

engine = sqlmodel.create_engine(DB_URL, echo=False)


class Database:
    """Handles database setup and transaction insertion."""

    def __init__(self):
        self._init_db()

    def _init_db(self):
        """Create db file if it doesn't exist"""
        db_path = pathlib.Path(DB_FILE)
        if not db_path.exists():
            db_path.parent.mkdir(parents=True, exist_ok=True)
            db_path.touch()

        # Create the database tables
        sqlmodel.SQLModel.metadata.create_all(engine)

    def insert_transaction(self, transaction: models.Transaction):
        """Insert a transaction into the DB."""
        with sqlmodel.Session(engine) as session:
            session.add(transaction)
            session.commit()
