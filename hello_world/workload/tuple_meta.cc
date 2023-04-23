#include "tuple_meta.h"

TupleMeta::TupleMeta(uint64_t lock_txn_id_, uint64_t ts_) {
  lock_txn_id = lock_txn_id_;
  ts = ts_;
}