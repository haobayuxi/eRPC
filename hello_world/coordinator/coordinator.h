
#ifndef COORDINATOR_H
#define COORDINATOR_H
#include "../common.h"
#include "../msg.h"
#include "rpc.h"

using namespace std;

class Coordinator {
 public:
  Coordinator(int id_, int server_num_, int server_threads_,
              vector<RemoteNode> server_addrs_);
  int id;

  int server_num;
  int server_threads;
  vector<RemoteNode> server_addrs;
  vector<vector<int>> sessions;
  erpc::Rpc<erpc::CTransport> *rpc_ = nullptr;
  erpc::MsgBuffer req;
  erpc::MsgBuffer resp;
  size_t start_tsc_;

  //   txn info
  uint64_t t_id;
  vector<struct Key> read_only_set;
  vector<struct Key> read_write_set;
  int reply_num;
  //
  void init_rpc();

  // send msgs
  void txn_begin();
  void txn_execute();
  void txn_validate();
  void txn_abort();
  void txn_commit();

  // handle res
  void handle_execution_resp(struct ExecutionRes res);
  int t;
  int num_sm_resps;

 private:
};

void run_coordinator(Coordinator *c, erpc::Nexus *nexus);

ALWAYS_INLINE
void Coordinator::AddToReadOnlySet(DataItemPtr item) {
  DataSetItem data_set_item{.item_ptr = std::move(item),
                            .is_fetched = false,
                            .is_logged = false,
                            .read_which_node = -1,
                            .bkt_idx = -1};
  read_only_set.emplace_back(data_set_item);
}

ALWAYS_INLINE
void Coordinator::AddToReadWriteSet(DataItemPtr item) {
  DataSetItem data_set_item{.item_ptr = std::move(item),
                            .is_fetched = false,
                            .is_logged = false,
                            .read_which_node = -1,
                            .bkt_idx = -1};
  read_write_set.emplace_back(data_set_item);
}

#endif