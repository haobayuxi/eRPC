
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
  auto *c = static_cast<MemServer *>(_handler);
  const erpc::MsgBuffer *req_buff = req_handle->get_req_msgbuf();
  size_t req_size = req_buff->get_data_size();
  //   get request
  auto req = new Execution();
  req->unpack();
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
  switch (db_type) {
    case DbType::Micro:
    default:
  }
}