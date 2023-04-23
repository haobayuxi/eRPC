#include "common.h"
#include "mem_server/mem_server.h"
erpc::Rpc<erpc::CTransport> *rpc;

void req_handler(erpc::ReqHandle *req_handle, void *) {
  auto &resp = req_handle->pre_resp_msgbuf_;
  rpc->resize_msg_buffer(&resp, kMsgSize);
  sprintf(reinterpret_cast<char *>(resp.buf_), "hello");
  // printf("got a message\n");
  rpc->enqueue_response(req_handle, &resp);
}

int main() {
  std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort);
  erpc::Nexus nexus(server_uri);
  nexus.register_req_func(kReqType, req_handler);
  size_t num_threads = 10;
  std::vector<std::thread> threads(num_threads);
  std::vector<MemServer *> mem_server_handlers;
  for (size_t i = 0; i < num_threads; i++) {
    MemServer *handler = new MemServer();
    threads[i] = std::thread(handler, &nexus);
    erpc::bind_to_core(threads[i], 0, i);
  }

  for (size_t i = 0; i < num_threads; i++) threads[i].join();
  // rpc = new erpc::Rpc<erpc::CTransport>(&nexus, nullptr, 0, nullptr);
  // rpc->run_event_loop(100000);
}
