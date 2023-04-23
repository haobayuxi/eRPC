#include <unistd.h>

#include <chrono>
#include <ctime>
#include <iostream>

#include "coordinator/coordinator.h"

using namespace std::chrono;

#include "common.h"
// erpc::Rpc<erpc::CTransport> *rpc;
erpc::MsgBuffer req;
erpc::MsgBuffer resp;
int session_num;
int count = 0;

class ClientContext {
 public:
  size_t start_tsc_;
  erpc::Rpc<erpc::CTransport> *rpc_ = nullptr;
};

// void cont_func(void *_context, void *) {
//   auto *c = static_cast<ClientContext *>(_context);
//   const double req_lat_us =
//       erpc::to_usec(erpc::rdtsc() - c->start_tsc_, c->rpc_->get_freq_ghz());
//   std::cout << req_lat_us << std::endl;
//   printf("%s\n", resp.buf_);
//   c->start_tsc_ = erpc::rdtsc();
//   count += 1;
//   if (count > 10) {
//     return;
//   }
//   c->rpc_->enqueue_request(session_num, kReqType, &req, &resp, cont_func,
//   NULL);
// }

void sm_handler(int, erpc::SmEventType, erpc::SmErrType, void *) {}
int main() {
  std::string client_uri = kClientHostname + ":" + std::to_string(kUDPPort);
  erpc::Nexus nexus(client_uri);

  vector<RemoteNode> remotes;
  struct RemoteNode s;
  s.ip = kServerHostname;
  s.port = kUDPPort;
  remotes.push_back(s);
  Coordinator *c = new Coordinator(0, 1, 10, remotes);
  run_coordinator(c);
  // ClientContext c;
  // erpc::Rpc<erpc::CTransport> *rpc = new erpc::Rpc<erpc::CTransport>(
  //     &nexus, static_cast<void *>(&c), 0, sm_handler);
  // // erpc::Rpc<erpc::CTransport> rpc(nexus, static_cast<void *>(&c), 0,
  // // sm_handler, 1);
  // c.rpc_ = rpc;
  // std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort);
  // session_num = rpc->create_session(server_uri, 0);

  // while (!rpc->is_connected(session_num)) rpc->run_event_loop_once();

  // req = rpc->alloc_msg_buffer_or_die(kMsgSize);
  // resp = rpc->alloc_msg_buffer_or_die(kMsgSize);
  // // for (int i = 0; i < 10; i++) {
  // c.start_tsc_ = erpc::rdtsc();
  // rpc->enqueue_request(session_num, kReqType, &req, &resp, cont_func, NULL);
  // }
  // while (1) {
  //   rpc->run_event_loop(10000);
  // }

  sleep(1);

  // delete rpc;
}
