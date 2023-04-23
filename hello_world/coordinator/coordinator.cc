
#include "coordinator.h"

void coordinator_sm_handler(int session_num, erpc::SmEventType sm_event_type,
                            erpc::SmErrType sm_err_type, void *_context) {
  auto *c = static_cast<Coordinator *>(_context);
  c->num_sm_resps++;

  erpc::rt_assert(
      sm_err_type == erpc::SmErrType::kNoError,
      "SM response with error " + erpc::sm_err_type_str(sm_err_type));

  if (!(sm_event_type == erpc::SmEventType::kConnected ||
        sm_event_type == erpc::SmEventType::kDisconnected)) {
    throw std::runtime_error("Received unexpected SM event.");
  }
}

void cont_func(void *_context, void *_session) {
  auto *c = static_cast<Coordinator *>(_context);
  auto session = reinterpret_cast<int>(_session);
  c->t += 1;
  if (c->t >= 10) {
    return;
  }
  int session_num = c->sessions[0][c->t];
  printf("session = %d %d, value = %s\n", session, session_num, c->resp.buf_);
  c->rpc_->enqueue_request(session_num, kReqType, &c->req, &c->resp, cont_func,
                           reinterpret_cast<void *>(session_num));
}

void run_coordinator(Coordinator *c, erpc::Nexus *nexus) {
  erpc::Rpc<erpc::CTransport> rpc(nexus, static_cast<void *>(c), 0,
                                  coordinator_sm_handler, 0);
  c->rpc_ = &rpc;
  //   for (size_t i = 0; i < c->server_threads; i++) {
  c->req = rpc.alloc_msg_buffer_or_die(Max_Msg_Size);
  c->resp = rpc.alloc_msg_buffer_or_die(Max_Msg_Size);
  //   }

  c->init_rpc();
  c->start_tsc_ = erpc::rdtsc();
  int session_num = c->sessions[0][0];
  c->rpc_->enqueue_request(session_num, kReqType, &c->req, &c->resp, cont_func,
                           reinterpret_cast<void *>(session_num));
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
  t = 0;
}

void Coordinator::init_rpc() {
  // init connect to servers
  for (size_t i = 0; i < server_num; i++) {
    std::string server_uri =
        server_addrs[i].ip + ":" + std::to_string(server_addrs[i].port);
    vector<int> sessions_per_server;
    for (size_t j = 0; j < server_threads; j++) {
      int session_num = rpc_->create_session(server_uri, j);
      sessions_per_server.push_back(session_num);
    }

    sessions.push_back(sessions_per_server);
  }

  while (num_sm_resps != server_num * server_threads) {
    rpc_->run_event_loop(100);
  }
  printf("init rpc done\n");
}