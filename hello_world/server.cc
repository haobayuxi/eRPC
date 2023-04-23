#include "common.h"
#include "mem_server/mem_server.h"
#include "util/latency.h"
#include "util/math_utils.h"
#include "util/numautils.h"

// erpc::Rpc<erpc::CTransport> *rpc;

void reqs_handler(erpc::ReqHandle *req_handle, void *_handler) {
  auto *c = static_cast<MemServer *>(_handler);
  auto &resp = req_handle->pre_resp_msgbuf_;
  c->rpc->resize_msg_buffer(&resp, kMsgSize);
  sprintf(reinterpret_cast<char *>(resp.buf_), "hello");
  // printf("got a message\n");
  c->rpc->enqueue_response(reqs_handler, &resp);
}

int main() {
  std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort);
  erpc::Nexus nexus(server_uri);
  nexus.register_req_func(kReqType, req_handler);
  size_t num_threads = 1;
  std::vector<std::thread> threads(num_threads);
  for (size_t i = 0; i < num_threads; i++) {
    MemServer *handler = new MemServer(i);
    // threads[i] = std::thread(run_server, handler, &nexus);
    // erpc::bind_to_core(threads[i], 0, i);
    run_server(handler, &nexus);
  }

  // for (size_t i = 0; i < num_threads; i++) threads[i].join();
  // rpc = new erpc::Rpc<erpc::CTransport>(&nexus, nullptr, 0, nullptr);
  // rpc->run_event_loop(100000);
}
