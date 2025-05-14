"""Database operations for transaction data."""

import pathlib
from typing import Any

import sqlmodel
from sqlalchemy import text
from sqlalchemy.engine import Engine

import models


DB_FILE = "data/transactions.db"
DB_URL = f"sqlite:///{DB_FILE}"


def get_real_engine():
    """Create a real SQLAlchemy engine."""
    db_path = pathlib.Path(DB_FILE)
    if not db_path.exists():
        db_path.parent.mkdir(parents=True, exist_ok=True)
        db_path.touch()

    return sqlmodel.create_engine(DB_URL, echo=False)


class TransactionDB:
    """Handles database setup and transaction insertion."""

    def __init__(self, engine: Engine | None = None):
        if engine is None:
            self.engine = get_real_engine()
        else:
            self.engine = engine
        
        sqlmodel.SQLModel.metadata.create_all(self.engine)

    def insert_transaction(self, transaction: models.Transaction):
        """Insert a transaction into the DB."""
        with sqlmodel.Session(self.engine) as session:
            session.add(transaction)
            session.commit()
    
    def insert_transactions(self, transactions: list[models.Transaction]):
        """Insert multiple transactions into the DB."""
        with sqlmodel.Session(self.engine) as session:
            session.add_all(transactions)
            session.commit()
    
    def most_recent_transaction(self, security_id: str) -> models.Transaction | None:
        """Get the most recent transaction for a security.

        Args:
            security_id: The ID of the security to look up.

        Returns:
            The most recent Transaction object, or None if no transactions exist.
        """
        with sqlmodel.Session(self.engine) as session:
            query = (
                sqlmodel.select(models.Transaction)
                .where(models.Transaction.security_id == security_id)
                .order_by(models.Transaction.timestamp.desc())
                .limit(1)
            )
            result = session.exec(query).first()
            return result
    
    def historical_prices(self, security_id: str, step: str) -> list[dict[str, Any]]:
        """Get historical price data for a security aggregated by time step.

        Args:
            security_id: The ID of the security to look up.
            step: The time step to aggregate by. One of 'min', 'hr', 'day', 'month'.

        Returns:
            A list of dictionaries containing period, avg_price, min_price, max_price,
            total_volume, and num_transactions.

        Raises:
            ValueError: If step is not one of the valid options.
        """
        valid_steps = {'min', 'hr', 'day', 'month'}
        if step not in valid_steps:
            raise ValueError(f"Step must be one of {valid_steps}, got {step!r}")

        # Map step to SQLite strftime format string
        format_map = {
            'min': '%Y-%m-%d %H:%M',
            'hr': '%Y-%m-%d %H',
            'day': '%Y-%m-%d',
            'month': '%Y-%m'
        }
        format_str = format_map[step]
        
        with sqlmodel.Session(self.engine) as session:
            query = text("""
                SELECT 
                    strftime(:format, timestamp) as period,
                    AVG(price) as avg_price,
                    MIN(price) as min_price,
                    MAX(price) as max_price,
                    SUM(quantity) as total_volume,
                    COUNT(*) as num_transactions
                FROM "transaction"
                WHERE security_id = :security_id
                GROUP BY period
                ORDER BY period
            """)
            
            result = session.exec(query.bindparams(
                format=format_str,
                security_id=security_id
            ))
            
            # Convert result to list of dicts
            return [
                {
                    "period": row[0],
                    "avg_price": row[1],
                    "min_price": row[2],
                    "max_price": row[3],
                    "total_volume": row[4],
                    "num_transactions": row[5]
                }
                for row in result
            ]
    
    def clear_all_transactions(self):
        """Clear all transactions from the database.
        
        This is primarily useful for testing.
        """
        with sqlmodel.Session(self.engine) as session:
            session.exec(text("DELETE FROM \"transaction\""))
            session.commit()
