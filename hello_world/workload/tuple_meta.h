#ifndef TUPLEMETA_H
#define TUPLEMETA_H
#include <pthread.h>
#include <stdint.h>
#include <string.h>

using namespace std;

class TupleMeta {
 public:
  TupleMeta() {
    lock_txn_id = 0;
    ts = 0;
    rwlock = PTHREAD_RWLOCK_INITIALIZER;
  }
  uint64_t lock_txn_id;
  uint64_t ts;
  pthread_rwlock_t rwlock;
  int get_read_lock() { return pthread_rwlock_rdlock(&rwlock); }
  int get_write_lock() { return pthread_rwlock_wrlock(&rwlock); }
  void release_rw_lock() { pthread_rwlock_unlock(&rwlock); }
  bool is_locked() {
    if (lock_txn_id == 0) {
      return false;
    }
    return true;
  }
  bool set_lock(uint64_t txn_id) {
    if (lock_txn_id == 0) {
      lock_txn_id = txn_id;
      return true;
    }
    return false;
  }
  void release_lock(uint64_t txn_id) {
    if (lock_txn_id == txn_id) {
      lock_txn_id = 0;
    }
  }
};

#endif