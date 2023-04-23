
#include "mem_server.h"

#include <iostream>

void run_server(MemServer *s, erpc::Nexus *nexus) {
  std << out << "thread id = " << s->thread_id << std::endl;
  erpc::Rpc<erpc::CTransport> rpc(nexus, static_cast<void *>(s), s->thread_id,
                                  NULL);
  s->rpc_ = &rpc;
  while (true) {
    rpc.run_event_loop(100);
  }
}

MemServer::MemServer(size_t _thread_id) { thread_id = _thread_id; }