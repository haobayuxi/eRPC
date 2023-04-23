
#include "mem_server.h"

void run_server(MemServer *s, erpc::Nexus *nexus) {
  printf("thread id = %d\n", s->thread_id);
  erpc::Rpc<erpc::CTransport> rpc(nexus, static_cast<void *>(s), s->thread_id,
                                  NULL);
  s->rpc_ = &rpc;
  while (true) {
    rpc.run_event_loop(100);
  }
}

MemServer::MemServer(size_t _thread_id) { thread_id = _thread_id; }