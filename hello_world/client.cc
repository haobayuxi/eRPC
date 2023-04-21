#include <unistd.h>

#include <chrono>
#include <ctime>
#include <iostream>

using namespace std::chrono;

#include "common.h"
erpc::Rpc<erpc::CTransport> *rpc;
erpc::MsgBuffer req;
erpc::MsgBuffer resp;
int session_num;

// class Context {
//   public:

// }

void cont_func(void *, void *start) {
  auto start_time = *reinterpret_cast<system_clock::time_point *>(start);
  auto end_time = system_clock::now();
  auto microseconds_since_epoch =
      duration_cast<microseconds>(end_time - start_time)
          .count();  // 将时长转换为微秒数
  std::cout << microseconds_since_epoch << std::endl;
  printf("%s\n", resp.buf_);
  start_time = system_clock::now();
  rpc->enqueue_request(session_num, kReqType, &req, &resp, cont_func,
                       reinterpret_cast<void *>(&start_time));
}

void sm_handler(int, erpc::SmEventType, erpc::SmErrType, void *) {}

int main() {
  std::string client_uri = kClientHostname + ":" + std::to_string(kUDPPort);
  erpc::Nexus nexus(client_uri);

  rpc = new erpc::Rpc<erpc::CTransport>(&nexus, nullptr, 0, sm_handler);

  std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort);
  session_num = rpc->create_session(server_uri, 0);

  while (!rpc->is_connected(session_num)) rpc->run_event_loop_once();

  req = rpc->alloc_msg_buffer_or_die(kMsgSize);
  resp = rpc->alloc_msg_buffer_or_die(kMsgSize);
  // for (int i = 0; i < 10; i++) {
  auto start_time = system_clock::now();
  rpc->enqueue_request(session_num, kReqType, &req, &resp, cont_func,
                       reinterpret_cast<void *>(&start_time));
  // }
  while (1) {
    rpc->run_event_loop(10000);
  }

  sleep(1);

  delete rpc;
}
