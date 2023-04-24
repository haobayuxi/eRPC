#ifndef MSG
#define MSG
#include <vector>

#include "common.h"
// rpc operation
#define ThroughputType 0
#define ExecutionType 1
#define ValidationType 2
#define CommitType 3

struct Key {
  uint32_t table_id;
  uint64_t key;
};

struct DataItem {
  struct Key key;
  uint64_t ts;
  uint8_t value[MAX_ITEM_SIZE];
  int data_size;
};
using DataItemPtr = std::shared_ptr<DataItem>;

class ExecutionRequest {
 public:
  uint64_t txn_id;
  std::vector<struct Key> read_set;
  std::vector<struct Key> write_set;
};

void unpack_exe_request(const erpc::MsgBuffer *req_msgbuf,
                        ExecutionRequest *request);

class ExecutionRes {
 public:
  uint64_t txn_id;
  std::vector<DataItem> read_set;
  bool success;
};

void unpack_exe_response(erpc::MsgBuffer &req_msgbuf, ExecutionRes *response);

class ValidationRequest {
 public:
  uint64_t txn_id;
  std::vector<uint64_t> read_set;
};

class ValidationRes {
 public:
  uint64_t txn_id;
  bool success;
};

class CommitRequest {
 public:
  uint64_t txn_id;
  std::vector<uint64_t> write_set;
};

class CommitRes {
 public:
  bool success;
};

class AbortRequest {
 public:
  uint64_t txn_id;
  std::vector<uint64_t> write_set;
};

#endif
