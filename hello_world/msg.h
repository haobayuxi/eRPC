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
};
using DataItemPtr = std::shared_ptr<DataItem>;

class ExecutionRequest {
 public:
  uint64_t txn_id;
  std::vector<struct Key> read_set;
  std::vector<struct Key> write_set;
};

void unpack_exe_request(const erpc::MsgBuffer *req_msgbuf,
                        ExecutionRequest *request) {
  uint8_t *buf = req_msgbuf->buf_;
  memcpy(&request->txn_id, buf, 8);
  buf += 8;
  int read_set_size = 0;
  memcpy(&read_set_size, buf, 4);
  buf += 4;
  auto len = sizeof(struct Key);
  for (int i = 0; i < read_set_size; i++) {
    struct Key key;
    memcpy(&key, buf, len);
    request->read_set.push_back(key);
    buf += len;
  }
  int write_set_size = 0;
  memcpy(&write_set_size, buf, 4);
  buf += 4;
  for (int i = 0; i < write_set_size; i++) {
    struct Key key;
    memcpy(&key, buf, len);
    request->write_set.push_back(key);
    buf += len;
  }
}

class ExecutionRes {
 public:
  uint64_t txn_id;
  std::vector<uint64_t> read_set;
  bool success;

  void unpack_exe_response(erpc::MsgBuffer &req_msgbuf) {
    uint8_t *buf = req_msgbuf.buf_;
    memcpy(&txn_id, buf, 8);
    buf += 8;
  }
};

void serialize_exe_response(erpc::MsgBuffer &req_msgbuf,
                            ExecutionRes *request) {
  uint8_t *buf = req_msgbuf.buf_;
  // memcpy(buf, &txn_id, 8);
  // buf += 8;
  // uint32_t read_set_size = read_set.size();
  // memcpy(buf, &read_set_size, 4);
  // buf += 4;
  // for (int i = 0; i <) memcpy(buf, &txn_id, 8);
}

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
