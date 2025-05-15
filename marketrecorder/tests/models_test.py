from datetime import datetime

import pydantic
import pytest

from src import models


class TestTransaction:
    @classmethod
    def setup_class(cls):
        cls._valid_transaction_data = (
            "MSFT|390.05|2.00|fab|piero|2025-05-04T21:13:31"
        )

    def test_parse_transaction_success(self):
        got = models.parse_transaction(self._valid_transaction_data)

        expected = models.Transaction(
            id=got.id,
            security_id="MSFT",
            price=390.05,
            quantity=2.00,
            buyer="fab",
            seller="piero",
            timestamp=datetime.fromisoformat("2025-05-04T21:13:31"),
        )

        assert got == expected, f"Expected {expected}, but got {got}"

    def test_parse_transaction_invalid_format_fails(self):
        invalid_data = "MSFT|390.05|2.00|fab|piero|2025-05-04T21:13:31|extra_field"
        with pytest.raises(ValueError):
            models.parse_transaction(invalid_data)

    def test_parse_transaction_invalid_type_fails(self):
        invalid_data = "MSFT|390.05|two|fab|piero|2025-05-04T21:13:31"
        with pytest.raises(pydantic.ValidationError):
            models.parse_transaction(invalid_data)
