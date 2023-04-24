#pragma once
#include <iostream>

#include "../msg.h"

using namespace std;

class DataStore {
 public:
  SystemType type;
  bool get_read_set(ExecutionRequest *request, ExecutionRes *response) {
    return true;
  }
  bool validate_read_set(ValidationRequest *request) { return true; }
  bool lock_write_set(ExecutionRequest *request) { return true; }
  bool update_and_release_locks(CommitRequest *c) { return true; }
  bool release_locks(AbortRequest *abort) { return true; }
};
