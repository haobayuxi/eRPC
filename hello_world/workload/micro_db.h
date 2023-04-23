#include <string.h>

#include <unordered_map>

#include "tuple_meta.h"
#include "workload.h"

using namespace std;
class Micro_Db : public DataStore {
 public:
  unordered_map<uint64_t, TupleMeta> data;
  bool get_read_set(ExecutionRes *res) { return true; }
  bool validate_read_set(ValidationRes *res) { return true; }
  bool lock_write_set(ExecutionRes *res) { return true; }
  bool update_and_release_locks(Commit *c) { return true; }
  bool release_locks(Abort *abort) { return true; }
};
