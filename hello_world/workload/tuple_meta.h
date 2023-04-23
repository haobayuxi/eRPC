#ifndef TUPLEMETA_H
#define TUPLEMETA_H
#include <stdint.h>
#include <string.h>

using namespace std;

enum class Workload_Value {

};

class TupleMeta {
 public:
  TupleMeta(uint64_t lock_txn_id_, uint64_t ts_);
  uint64_t lock_txn_id;
  uint64_t ts;
  bool is_locked();
  bool set_lock(uint64_t txn_id);
  void release_lock(uint64_t txn_id);
};

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

#endif