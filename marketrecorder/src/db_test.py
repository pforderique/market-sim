"""Tests for the TransactionDB class."""

import datetime
import uuid
from unittest import mock

import pytest
import sqlmodel

import models
import db
from db import TransactionDB


@pytest.fixture
def sample_transactions():
    """Create sample transaction data for testing."""
    return [
        models.Transaction(
            id=uuid.UUID("00000000-0000-0000-0000-000000000001"),
            security_id="AAPL",
            price=180.50,
            quantity=10.0,
            buyer="alice",
            seller="bob",
            timestamp=datetime.datetime(2025, 5, 1, 9, 30, 0),
        ),
        models.Transaction(
            id=uuid.UUID("00000000-0000-0000-0000-000000000002"),
            security_id="AAPL",
            price=181.25,
            quantity=5.0,
            buyer="charlie",
            seller="alice",
            timestamp=datetime.datetime(2025, 5, 1, 9, 45, 0),
        ),
        models.Transaction(
            id=uuid.UUID("00000000-0000-0000-0000-000000000003"),
            security_id="AAPL",
            price=180.75,
            quantity=15.0,
            buyer="bob",
            seller="dave",
            timestamp=datetime.datetime(2025, 5, 1, 10, 15, 0),
        ),
        models.Transaction(
            id=uuid.UUID("00000000-0000-0000-0000-000000000004"),
            security_id="MSFT",
            price=390.05,
            quantity=2.0,
            buyer="fab",
            seller="piero",
            timestamp=datetime.datetime(2025, 5, 4, 21, 13, 31),
        ),
        models.Transaction(
            id=uuid.UUID("00000000-0000-0000-0000-000000000005"),
            security_id="MSFT",
            price=391.50,
            quantity=3.0,
            buyer="piero",
            seller="charlie",
            timestamp=datetime.datetime(2025, 5, 4, 21, 14, 22),
        ),
    ]


@pytest.fixture
def test_db():
    """Create an in-memory SQLite database for testing."""
    # Use in-memory database for tests
    engine = sqlmodel.create_engine("sqlite:///:memory:", echo=False)

    # Create DB instance
    db = TransactionDB(engine=engine)

    # Return the DB instance
    return db


@pytest.fixture
def populated_db(test_db, sample_transactions):
    """Create a database with sample transactions."""
    # Clear any existing data
    test_db.clear_all_transactions()
    
    # Insert sample transactions
    test_db.insert_transactions(sample_transactions)
    
    return test_db


class TestTransactionDB:
    """Test cases for TransactionDB."""

    def test_insert_transaction(self, test_db):
        """Test inserting a single transaction."""
        tx = models.Transaction(
            security_id="GOOG",
            price=150.75,
            quantity=3.0,
            buyer="user1",
            seller="user2",
            timestamp=datetime.datetime(2025, 5, 10, 12, 0, 0),
        )
        
        test_db.insert_transaction(tx)
        
        # Check it was inserted
        with sqlmodel.Session(test_db.engine) as session:
            result = session.exec(
                sqlmodel.select(models.Transaction).where(
                    models.Transaction.security_id == "GOOG"
                )
            ).first()
            
            assert result is not None
            assert result.security_id == "GOOG"
            assert result.price == 150.75
            assert result.buyer == "user1"
            assert result.seller == "user2"
    
    def test_insert_transactions(self, test_db):
        """Test inserting multiple transactions at once."""
        txs = [
            models.Transaction(
                security_id="TSLA",
                price=250.50,
                quantity=2.0,
                buyer="user3",
                seller="user4",
                timestamp=datetime.datetime(2025, 5, 11, 9, 0, 0),
            ),
            models.Transaction(
                security_id="TSLA",
                price=251.00,
                quantity=1.5,
                buyer="user5",
                seller="user3",
                timestamp=datetime.datetime(2025, 5, 11, 9, 15, 0),
            )
        ]
        
        test_db.insert_transactions(txs)
        
        # Check they were inserted
        with sqlmodel.Session(test_db.engine) as session:
            results = session.exec(
                sqlmodel.select(models.Transaction).where(
                    models.Transaction.security_id == "TSLA"
                )
            ).all()
            
            assert len(results) == 2
            assert results[0].price == 250.50
            assert results[1].price == 251.00
    
    def test_most_recent_transaction(self, populated_db):
        """Test retrieving the most recent transaction for a security."""
        # Test with AAPL
        aapl_tx = populated_db.most_recent_transaction("AAPL")
        assert aapl_tx is not None
        assert aapl_tx.security_id == "AAPL"
        assert aapl_tx.timestamp == datetime.datetime(2025, 5, 1, 10, 15, 0)
        assert aapl_tx.price == 180.75
        
        # Test with MSFT
        msft_tx = populated_db.most_recent_transaction("MSFT")
        assert msft_tx is not None
        assert msft_tx.security_id == "MSFT"
        assert msft_tx.timestamp == datetime.datetime(2025, 5, 4, 21, 14, 22)
        assert msft_tx.price == 391.50
        
        # Test with non-existent security
        none_tx = populated_db.most_recent_transaction("NONEXISTENT")
        assert none_tx is None
    
    def test_historical_prices(self, populated_db):
        """Test historical price aggregation."""
        # Test by minute
        min_results = populated_db.historical_prices("AAPL", "min")
        assert len(min_results) > 0
        for result in min_results:
            assert isinstance(result["period"], str)
            assert isinstance(result["avg_price"], float)
            assert isinstance(result["total_volume"], float)
        
        # Test by hour
        hr_results = populated_db.historical_prices("AAPL", "hr")
        assert len(hr_results) > 0
        
        # Test by day
        day_results = populated_db.historical_prices("MSFT", "day")
        assert len(day_results) == 1
        assert day_results[0]["period"] == "2025-05-04"
        
        # Test by month
        month_results = populated_db.historical_prices("AAPL", "month")
        assert len(month_results) == 1
        assert month_results[0]["period"] == "2025-05"
        
        # Test invalid step
        with pytest.raises(ValueError):
            populated_db.historical_prices("AAPL", "invalid")

    def test_clear_all_transactions(self, populated_db):
        """Test clearing all transactions."""
        # Verify data exists before clearing
        with sqlmodel.Session(populated_db.engine) as session:
            count_before = session.exec(sqlmodel.select(sqlmodel.func.count()).select_from(models.Transaction)).one()
            assert count_before > 0
        
        # Clear transactions
        populated_db.clear_all_transactions()
        
        # Verify data is gone
        with sqlmodel.Session(populated_db.engine) as session:
            count_after = session.exec(sqlmodel.select(sqlmodel.func.count()).select_from(models.Transaction)).one()
            assert count_after == 0
