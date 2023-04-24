#ifndef MSG
#define MSG
#include <vector>

#include "common.h"

using namespace std;

// rpc operation
#define ThroughputType 0
#define ExecutionType 1
#define ValidationType 2
#define CommitType 3

#define KeySize 12
#define DataItemSize 60
struct Key {
  uint32_t table_id;
  uint64_t key;
};

struct DataItem {
  Key key;
  uint64_t ts;
  uint8_t value[MAX_ITEM_SIZE];
};

class ExecutionRequest {
 public:
  uint64_t txn_id;
  std::vector<Key> read_set;
  std::vector<Key> write_set;
};

void serialize_exe_request(erpc::MsgBuffer *req_msgbuf,
                           vector<DataItem *> *read_set,
                           vector<DataItem *> *write_set);

void unpack_exe_request(erpc::MsgBuffer *req_msgbuf, ExecutionRequest *request);

class ExecutionRes {
 public:
  bool success;
  uint64_t txn_id;
  std::vector<DataItem> read_set;
};

void serialize_exe_response(erpc::MsgBuffer *req_msgbuf,
                            ExecutionRes *response);

void unpack_exe_response(erpc::MsgBuffer &req_msgbuf, ExecutionRes *response);

class ValidationRequest {
 public:
  uint64_t txn_id;
  std::vector<Key> read_set;
  std::vector<uint64_t> read_ts_set;
  std::vector<Key> write_set;
};

void unpack_validate_request(erpc::MsgBuffer &req_msgbuf,
                             ValidationRequest *request);

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
