
#include "../common.h"
#include "../msg.h"
#include "rpc.h"

#define Max_Server_Thread 30

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

 private:
  int num_sm_resps;
};

void run_coordinator(Coordinator *c);

void basic_sm_handler(int session_num, erpc::SmEventType sm_event_type,
                      erpc::SmErrType sm_err_type, void *_context) {
  auto *c = static_cast<Coordinator *>(_context);
  c->num_sm_resps_++;

  erpc::rt_assert(
      sm_err_type == erpc::SmErrType::kNoError,
      "SM response with error " + erpc::sm_err_type_str(sm_err_type));

  if (!(sm_event_type == erpc::SmEventType::kConnected ||
        sm_event_type == erpc::SmEventType::kDisconnected)) {
    throw std::runtime_error("Received unexpected SM event.");
  }
}