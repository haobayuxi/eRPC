
#include "../common.h"
#include "../msg.h"
#include "rpc.h"

#define Max_Server_Thread 30

using namespace std;

class Coordinator {
 public:
  Coordinator();
  int id;
  uint64_t t_id;
  int server_num;
  int server_threads;
  vector<RemoteNode> server_addrs;
  vector<vector<int>> sessions;
  erpc::Rpc<erpc::CTransport> *rpc_ = nullptr;
  erpc::MsgBuffer req;
  erpc::MsgBuffer resp;
  //
  void init_rpc();

 private:
  int num_sm_resps;
};

void run_coordinator();