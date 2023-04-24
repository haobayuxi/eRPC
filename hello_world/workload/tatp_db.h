#include <stdint.h>

#include <unordered_map>

#include "../common/msg.h"
#include "tuple_meta.h"
#include "workload.h"

struct SubscriberValue {
  char sub_number_unused[15];
  char hex[5];
  char bytes[10];
  short bits;
  uint32_t msc_location;
  uint32_t vlr_location;
};

struct SubscriberTuple {
  TupleMeta meta;
  SubscriberValue value;
};

struct AccessInfoValue {
  char data1;
  char data2;
  char data3[3];
  char data4[5];
  uint8_t unused[6];
};

struct SpecialFacilityValue {};

struct CallForwardingValue {};

class Tatp_Db : public DataStore {
 public:
  unordered_map<uint64_t, SubscriberTuple> sub_table;

  bool get_read_set(ExecutionRequest *request, ExecutionRes *response) {
    return true;
  }
  bool validate_read_set(ValidationRequest *request) { return true; }
  bool lock_write_set(ExecutionRequest *request) { return true; }
  bool update_and_release_locks(CommitRequest *c) { return true; }
  bool release_locks(AbortRequest *abort) { return true; }
};