#include "tuple_meta.h"

TupleMeta::TupleMeta(uint64_t lock_txn_id_, uint64_t ts_) {
  lock_txn_id = lock_txn_id_;
  ts = ts_;
}

bool TupleMeta::is_locked() {
  if (lock_txn_id == 0) {
    return false;
  }
  return true;
}

bool TupleMeta::set_lock(uint64_t txn_id) {
  if (lock_txn_id == 0) {
    lock_txn_id = txn_id;
    return true;
  }
  return false;
}

void TupleMeta::release_lock(uint64_t txn_id) {
  if (lock_txn_id == txn_id) {
    lock_txn_id = 0;
  }
}