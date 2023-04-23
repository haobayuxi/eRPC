
#include "mem_server.h"

void run_server(MemServer *s, erpc::Nexus *nexus) {
  erpc::Rpc<erpc::CTransport> rpc(nexus, static_cast<void *>(&c), s->thread_id,
                                  basic_sm_handler);
  s->rpc_ = &rpc;
  while (true) {
    rpc.run_event_loop_once();
  }
}