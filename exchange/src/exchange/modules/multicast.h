// File: multicast.h
// UDP multicast module for transactions
#pragma once
#include <string>
#include "../../../datamodel/datamodel.h"

void multicast_transaction(const datamodel::Transaction &txn);
