
#include "coordinator.h"

void run_coordinator(Coordinator *c) {
  erpc::Rpc<erpc::CTransport> rpc(nexus, static_cast<void *>(&c), thread_id,
                                  basic_sm_handler, phy_port);
  c->rpc_ = &rpc;
  req = rpc.alloc_msg_buffer_or_die(Max_Msg_Size);
  resp = rpc.alloc_msg_buffer_or_die(Max_Msg_Size);
  c->init_rpc();
}

Coordinator::Coordinator(int id_, uint64_t t_id_, int server_num_,
                         int server_threads_,
                         vector<RemoteNode> server_addrs_) {
  id = id_;
  t_id = t_id_;
  server_num = server_num_;
  server_threads = server_threads_;
  server_addrs = server_addrs_;
  num_sm_resps = 0;
}

void Coordinator::init_rpc() {
  // init connect to servers
  for (int i = 0; i < server_num; i++) {
    std::string server_uri =
        server_addrs[i].ip + ":" + std::to_string(server_addrs[i].port);
    vector<int> sessions_per_server;
    for (int j = 0; j < server_threads; j++) {
      int session_num = rpc_->create_session(server_uri, j);
      sessions_per_server.push_back(session_num);
    }

    sessions.push_back(sessions_per_server);
  }

  while (c.num_sm_resps != server_num * server_threads) {
    c.rpc->run_event_loop(100);
  }
}