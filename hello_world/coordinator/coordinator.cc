
#include "coordinator.h"

void cont_func(void *_context, void *_session) {
  auto *c = static_cast<Coordinator *>(_context);
  auto *session = static_cast<size_t *>(_session);
  printf("session = %d, value = %s\n", *session, c->resp[*session].req);
}

void run_coordinator(Coordinator *c, erpc::Nexus *nexus) {
  erpc::Rpc<erpc::CTransport> rpc(nexus, static_cast<void *>(&c), 0,
                                  basic_sm_handler, NULL);
  c->rpc_ = &rpc;
  for (int i = 0; i < c->server_threads; i++) {
    c->req.push_back(rpc.alloc_msg_buffer_or_die(Max_Msg_Size));
    c->resp.push_back(rpc.alloc_msg_buffer_or_die(Max_Msg_Size));
  }

  c->init_rpc();
  c->start_tsc_ = erpc::rdtsc();
  for (int i = 0; i < c->server_num; i++) {
    for (int j = 0; j < c->server_threads; j++) {
      int session_num = c->sessions[i][j];

      c->rpc_->enqueue_request(session_num, kReqType, &c->req[j], &c->resp[j],
                               cont_func, reinterpret_cast<void *> session_num);
    }
  }
  while (1) {
    c->rpc_->run_event_loop(10000);
  }
}

Coordinator::Coordinator(int id_, int server_num_, int server_threads_,
                         vector<RemoteNode> server_addrs_) {
  id = id_;
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