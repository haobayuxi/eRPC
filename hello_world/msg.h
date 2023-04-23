
#include <vector>

// rpc operation
#define ExecutionType 1
#define ValidationType 2
#define CommitType 3

struct Key {
  uint32_t table_id;
  uint64_t key;
};

class Execution {
 public:
  uint64_t txn_id;
  std::vector<uint64_t> read_set;
  std::vector<uint64_t> write_set;

  static void serialize(erpc::MsgBuffer &req_msgbuf) {
    uint8_t *buf = req_msgbuf.buf_;
    // memcpy(buf, &txn_id, 8);
    // buf += 8;
    // uint32_t read_set_size = read_set.size();
    // memcpy(buf, &read_set_size, 4);
    // buf += 4;
    // for (int i = 0; i <) memcpy(buf, &txn_id, 8);
  }

  static void unpack(erpc::MsgBuffer &req_msgbuf) {
    // uint8_t *buf = req_msgbuf.buf_;
    // memcpy(, txn_id)
  }
};

class ExecutionRes {
 public:
  uint64_t txn_id;
  std::vector<uint64_t> read_set;
  bool success;
};

class Validation {
 public:
  uint64_t txn_id;
  std::vector<uint64_t> read_set;
};

class ValidationRes {
 public:
  uint64_t txn_id;
  bool success;
};

class Commit {
 public:
  uint64_t txn_id;
  std::vector<uint64_t> write_set;
};

class CommitRes {
 public:
  bool success;
};

class Abort {
 public:
  uint64_t txn_id;
  std::vector<uint64_t> write_set;
}