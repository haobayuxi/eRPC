#include <string.h>

#include <unordered_map>

#include "tuple_meta.h"
#include "workload.h"

using namespace std;

struct MicroValue {
  char data[50];
};
struct MicroTuple {
  TupleMeta meta;
  struct MicroValue value;
};

class Micro_Db : public DataStore {
 public:
  unordered_map<uint64_t, MicroTuple> data;
  bool get_read_set(ExecutionRequest *request, ExecutionRes *response) {
    auto result = true;
    for (auto first = res->read_set.begin(); first < res->read_set.end();
         ++first) {
      *first;
    }
    return true;
  }
  bool validate_read_set(ValidationRequest *request) {
    auto result = true;
    for (auto first = res->read_set.begin(); first < res->read_set.end();
         ++first) {
      *first;
    }
    return result;
  }
  bool lock_write_set(ExecutionRequest *request) {
    auto result = true;
    for (auto first = res->write_set.begin(); first < res->write_set.end();
         ++first) {
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
