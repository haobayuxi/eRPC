
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
  auto session = reinterpret_cast<size_t>(_session);

  int session_num = c->sessions[0][0];
  printf("session = %ld %d, value = %s\n", session, session_num, c->resp.buf_);
  c->rpc_->enqueue_request(session_num, ExecutionType, &c->req, &c->resp,
                           handle_execute_resp, NULL);
}

void handle_execute_resp(void *_context, void *) {
  auto *c = static_cast<Coordinator *>(_context);
  auto response = new ExecutionRes();
  unpack_exe_response(&c->resp, response);
  const double req_lat_us =
      erpc::to_usec(erpc::rdtsc() - c->start_tsc_, c->rpc_->get_freq_ghz());
  printf("txnid = %ld %d, success = %d ,latency = %lf\n", response->txn_id,
         response->read_set.size(), response->success, req_lat_us);
  //   c->rpc_->enqueue_request(session_num, kReqType, &c->req, &c->resp,
  //   cont_func,)
}

// void Coordinator::handle_validate_res(void *_context, void *_session) {
//   auto *c = static_cast<Coordinator *>(_context);
//   auto session = reinterpret_cast<size_t>(_session);
//   c->t += 1;
//   if (c->t >= 10) {
//     return;
//   }
//   int session_num = c->sessions[0][c->t];
//   printf("session = %ld %d, value = %s\n", session, session_num,
//   c->resp.buf_); c->rpc_->enqueue_request(session_num, kReqType, &c->req,
//   &c->resp, cont_func,
// }

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
  c->txn_id = 101;
  auto item = new DataItem();
  item->key.key = 10;
  item->key.table_id = 2;
  c->read_only_set.push_back(item);
  serialize_exe_request(&c->req, &c->read_only_set, &c->read_write_set,
                        c->txn_id);
  c->rpc_->enqueue_request(session_num, ExecutionType, &c->req, &c->resp,
                           handle_execute_resp, NULL);
  while (1) {
    c->rpc_->run_event_loop(10000);
  }
}

Coordinator::Coordinator(int id_, int server_num_, int server_threads_,
                         vector<RemoteNode> server_addrs_, LocalTs *local_ts_) {
  id = id_;
  server_num = server_num_;
  server_threads = server_threads_;
  server_addrs = server_addrs_;
  num_sm_resps = 0;
  local_ts = local_ts_;
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

void Coordinator::txn_begin() {
  reply_num = 0;
  start_ts = 0;
  have_readed = 0;
  have_read_writed = 0;
  // init workload

  // init start_time
  start_tsc_ = erpc::rdtsc();
  //   init start ts
  switch (type) {
    case Yuxi: {
      // get local ts
      break;
    }
    default: {
    }
  }
}
bool Coordinator::txn_execute() {
  // serialize execute msg
  bool read_only = true;
  if (read_write_set.size() > have_read_writed) {
    read_only = false;
  }
  if (read_only) {
    // get read set from a random server
  } else {
    // broadcast to all server
  }
  // send to servers

  return true;
}
bool Coordinator::txn_validate() {
  switch (type) {
    case Yuxi: {
    }
    case Meerkat: {
      // broadcast to validate
    }
    default: {
    }
  }
  // serialize validate msg
  // broadcast to servers
  return true;
}
void Coordinator::txn_abort() {
  // serialize abort msg
  switch (type) {
    case Yuxi: {
      if (read_write_set.size() != 0) {
        // send abort msg
      }
    }
    case Meerkat: {
      // broadcast to abort
    }
    default: {
    }
  }
}
void Coordinator::txn_commit() {
  // serialize commit msg
  switch (type) {
    case Yuxi: {
      if (read_write_set.size() != 0) {
        // send commit msg
      }
    }
    case Meerkat: {
      // broadcast to validate
    }
    default: {
    }
  }
}
