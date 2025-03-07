#include "common/common.h"
#include "common/msg.h"
#include "mem_server/mem_server.h"
#include "unordered_map"
#include "util/latency.h"
#include "util/math_utils.h"
#include "util/numautils.h"
#include "workload/micro_db.h"

using namespace std;

// erpc::Rpc<erpc::CTransport> *rpc;

void reqs_handler(erpc::ReqHandle *req_handle, void *_handler) {
  auto *c = static_cast<MemServer *>(_handler);
  auto &resp = req_handle->pre_resp_msgbuf_;
  c->rpc_->resize_msg_buffer(&resp, kMsgSize);
  std::string res = "hello" + std::to_string(c->thread_id);
  strcpy(reinterpret_cast<char *>(resp.buf_), res.c_str());
  // printf("got a message\n");
  c->rpc_->enqueue_response(req_handle, &resp);
}

int main() {
  std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort);
  erpc::Nexus nexus(server_uri);

  nexus.register_req_func(ExecutionType, handle_execute);
  size_t num_threads = 10;
  std::vector<std::thread> threads(num_threads);
  Micro_Db *store = new Micro_Db();
  for (size_t i = 0; i < num_threads; i++) {
    MemServer *handler = new MemServer(i, store);
    threads[i] = std::thread(run_server, handler, &nexus);
    erpc::bind_to_core(threads[i], 0, i);
  }

  for (size_t i = 0; i < num_threads; i++) threads[i].join();
  // rpc = new erpc::Rpc<erpc::CTransport>(&nexus, nullptr, 0, nullptr);
  // rpc->run_event_loop(100000);
}
