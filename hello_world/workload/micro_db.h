#include <string.h>

#include <unordered_map>

#include "tuple_meta.h"
#include "workload.h"

using namespace std;

#define MicroTableSize 100000
#define MicroDataSize 40

class MicroTuple {
 public:
  MicroTuple() { meta = TupleMeta(); }
  TupleMeta meta;
  char data[40];
};

class Micro_Db : public DataStore {
 public:
  Micro_Db() {
    for (uint64_t i = 0; i < MicroTableSize; i++) {
      data.insert(make_pair(i, new MicroTuple()));
    }
  }
  unordered_map<uint64_t, MicroTuple *> data;
  bool get_read_set(ExecutionRequest *request, ExecutionRes *response) {
    for (int i = 0; i < request->read_set.size(); i++) {
      // get tuple
      auto key = request->read_set[i].key;
      auto tuple = data[key];
      tuple->meta.get_read_lock();
      if (!tuple->meta.is_locked()) {
        // read
        struct DataItem item;
        item.key = request->read_set[i];
        item.ts = tuple->meta.ts;
        item.data_size = MicroDataSize;
        memcpy(item.data, tuple->data, MicroDataSize);
        response->read_set.push_back(item);
      } else {
        return false;
      }
    }
    return true;
  }
  bool validate_read_set(ValidationRequest *request) {
    auto result = true;
    for (auto first = request->read_set.begin();
         first < request->read_set.end(); ++first) {
      *first;
    }
    return result;
  }
  bool lock_write_set(ExecutionRequest *request) {
    auto result = true;
    for (auto first = request->write_set.begin();
         first < request->write_set.end(); ++first) {
      *first;
    }
    return result;
  }
  bool update_and_release_locks(CommitRequest *c) {
    for (auto first = c->write_set.begin(); first < c->write_set.end();
         ++first) {
      *first;
    }
    return true;
  }
  bool release_locks(AbortRequest *abort) {
    for (auto first = abort->write_set.begin(); first < abort->write_set.end();
         ++first) {
      *first;
    }
    return true;
  }
};
