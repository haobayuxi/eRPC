
#include <vector>

// rpc operation
#define ExecutionType 1
#define ValidationType 2
#define CommitType 3

struct Execution {
  uint64_t txn_id;
  std::vector<uint64_t> read_set;
};

struct ExecutionRes {
  uint64_t txn_id;
  std::vector<uint64_t> read_set;
  std::vector<uint64_t> write_set;
};

struct Validation {
  uint64_t txn_id;
  std::vector<uint64_t> read_set;
};

struct ValidationRes {
  uint64_t txn_id;
  bool success;
};

struct Commit {
  uint64_t txn_id;
};

struct CommitRes {
  bool success;
};