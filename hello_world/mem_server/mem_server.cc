
#include "mem_server.h"

void run_server(MemServer *s, erpc::Nexus *nexus) {
  erpc::Rpc<erpc::CTransport> rpc(nexus, static_cast<void *>(&c), thread_id,
                                  basic_sm_handler, port_vec.at(0));
  s->rpc_ = &rpc;
  while (true) {
    rpc.run_event_loop_once();
  }
}