// File: broadcast.h
// UDP broadcasting module for transactions
#pragma once
#include <string>
#include "../../../datamodel/datamodel.h"

void broadcast_transaction(const datamodel::Transaction &txn);
