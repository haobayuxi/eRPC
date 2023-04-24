
#include "mem_server.h"

#include <iostream>

void run_server(MemServer *s, erpc::Nexus *nexus) {
  std::cout << "thread id = " << s->thread_id << std::endl;
  erpc::Rpc<erpc::CTransport> rpc(nexus, static_cast<void *>(s), s->thread_id,
                                  NULL);
  s->rpc_ = &rpc;
  while (true) {
    rpc.run_event_loop(100);
  }
}

void handle_execute(erpc::ReqHandle *req_handle, void *_handler) {
  auto *server = static_cast<MemServer *>(_handler);
  const erpc::MsgBuffer *req_buff = req_handle->get_req_msgbuf();
  size_t req_size = req_buff->get_data_size();
  //   get request
  auto req = new ExecutionRequest();
  unpack_exe_request(req_buff, req);
  auto response = new ExecutionRes();
  response->txn_id = req->txn_id;
  //   get read data
  auto success = server->store->get_read_set(req, response);
  // lock write data
  if (!success) {
    // reply fail
    response->success = false;
    response->read_set.clear();
  } else {
    if (req->write_set.size() > 0) {
      success = server->store->lock_write_set(req);
      if (!success) {
        // reply fail
        response->success = false;
        response->read_set.clear();
      }
    } else {
      response->success = true;
    }
  }
  // serialize reponse

  // insert into wait list

  // reply to client
}

void handle_validate(erpc::ReqHandle *req_handle, void *_handler) {
  auto *c = static_cast<MemServer *>(_handler);
}

void handle_commit(erpc::ReqHandle *req_handle, void *_handler) {
  auto *c = static_cast<MemServer *>(_handler);
}

void handle_abort(erpc::ReqHandle *req_handle, void *_handler) {
  auto *c = static_cast<MemServer *>(_handler);
}

MemServer::MemServer(size_t _thread_id) {
  thread_id = _thread_id;
  // init data store
  switch (db_type) {
    case Micro: {
      //
      store = new Micro_Db();
      break;
    }
    default: {
    }
  }
}