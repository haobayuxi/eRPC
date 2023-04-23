
#include <iostream>

#include "../msg.h"

using namespace std;

class DataStore {
 public:
  SystemType type;
  bool get_read_set(ExecutionRes *res) { return true; }
  bool validate_read_set(ValidationRes *res) { return true; }
  bool lock_write_set(ExecutionRes *res) { return true; }
  bool update_and_release_locks(Commit *c) { return true; }
  bool release_locks(Abort *abort) { return true; }
};
