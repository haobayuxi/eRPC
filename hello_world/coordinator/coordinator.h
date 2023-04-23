
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
  uint64_t t_id;
  int server_num;
  int server_threads;
  vector<RemoteNode> server_addrs;
  vector<vector<int>> sessions;
  erpc::Rpc<erpc::CTransport> *rpc_ = nullptr;
  vector<erpc::MsgBuffer> req;
  vector<erpc::MsgBuffer> resp;
  size_t start_tsc_;
  //
  void init_rpc();

  int num_sm_resps;

 private:
};

void run_coordinator(Coordinator *c, erpc::Nexus *nexus);

#endif