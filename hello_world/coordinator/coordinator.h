
#ifndef COORDINATOR_H
#define COORDINATOR_H
#include <pthread.h>

#include "../common/common.h"
#include "../common/msg.h"
#include "rpc.h"
using namespace std;

class LocalTs {
 public:
  LocalTs() {
    rwlock = PTHREAD_RWLOCK_INITIALIZER;
    ts = 0;
  }
  uint64_t get_local_ts() {
    uint64_t result;
    pthread_rwlock_rdlock(&rwlock);
    result = ts;
    pthread_rwlock_unlock(&rwlock);
    return result;
  }

  bool set_local_ts(uint64_t ts_) {
    pthread_rwlock_wrlock(&rwlock);
    ts = ts_;
    pthread_rwlock_unlock(&rwlock);
    return true;
  }

 private:
  pthread_rwlock_t rwlock;
  uint64_t ts;
};
class Coordinator {
 public:
  Coordinator(int id_, int server_num_, int server_threads_,
              vector<RemoteNode> server_addrs_, LocalTs *local_ts_);
  int id;
  SystemType type;
  int server_num;
  int server_threads;
  vector<RemoteNode> server_addrs;
  //   rpc
  vector<vector<int>> sessions;
  erpc::Rpc<erpc::CTransport> *rpc_ = nullptr;
  erpc::MsgBuffer req;
  erpc::MsgBuffer resp;
  size_t start_tsc_;

  //   txn info
  uint64_t txn_id;
  uint64_t start_ts;
  LocalTs *local_ts;
  vector<DataItem *> read_only_set;
  vector<DataItem *> read_write_set;
  int have_readed;
  int have_read_writed;
  int reply_num;
  //
  void init_rpc();

  // send msgs
  void txn_begin();
  bool txn_execute();
  bool txn_validate();
  void txn_abort();
  void txn_commit();

  int num_sm_resps;

  //   serialize msg
  static void serialize_exe_request(erpc::MsgBuffer &req_msgbuf) {
    uint8_t *buf = req_msgbuf.buf_;
    // memcpy(buf, &txn_id, 8);
    // buf += 8;
    // uint32_t read_set_size = read_set.size();
    // memcpy(buf, &read_set_size, 4);
    // buf += 4;
    // for (int i = 0; i <) memcpy(buf, &txn_id, 8);
  }

 private:
};

void run_coordinator(Coordinator *c, erpc::Nexus *nexus);

void handle_execute_resp(void *_context, void *);

// ALWAYS_INLINE
// void Coordinator::AddToReadOnlySet(DataItemPtr item) {
//   DataSetItem data_set_item{.item_ptr = std::move(item),
//                             .is_fetched = false,
//                             .is_logged = false,
//                             .read_which_node = -1,
//                             .bkt_idx = -1};
//   read_only_set.emplace_back(data_set_item);
// }

// ALWAYS_INLINE
// void Coordinator::AddToReadWriteSet(DataItemPtr item) {
//   DataSetItem data_set_item{.item_ptr = std::move(item),
//                             .is_fetched = false,
//                             .is_logged = false,
//                             .read_which_node = -1,
//                             .bkt_idx = -1};
//   read_write_set.emplace_back(data_set_item);
// }

#endif