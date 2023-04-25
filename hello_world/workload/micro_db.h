#ifndef MICRODB_H
#define MICRODB_H
#include <string.h>

#include <unordered_map>

#include "tuple_meta.h"

using namespace std;

#define MicroTableSize 100000
#define MicroDataSize 40

class MicroTuple {
 public:
  MicroTuple() { meta = TupleMeta(); }
  TupleMeta meta;
  char data[MicroDataSize];
};

class Micro_Db {
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
        memcpy(item.value, tuple->data, MicroDataSize);
        response->read_set.push_back(item);
        tuple->meta.release_rw_lock();
      } else {
        // printf("tuple is locked\n");
        tuple->meta.release_rw_lock();
        return false;
      }
    }
    return true;
  }
  bool validate_read_set(ValidationRequest *request) {
    auto result = true;
    // for (int i = 0; i < request->read_set.size(); i++) {
    //   auto key = request->read_set[i];
    //   auto tuple = data[key.key];
    //   tuple->meta.get_read_lock();
    //   if (tuple->meta.is_locked()) {
    //     tuple->meta.release_rw_lock();
    //     return false;
    //   }
    // }
    return result;
  }
  bool lock_write_set(ExecutionRequest *request) {
    for (int i = 0; i < request->read_set.size(); i++) {
      auto key = request->read_set[i].key;
      auto tuple = data[key];
      if (!tuple->meta.set_lock(request->txn_id)) {
        return false;
      }
    }
    return true;
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

#endif
