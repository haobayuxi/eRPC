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

#endif