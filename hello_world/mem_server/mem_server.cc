
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
  printf("get a msg\n");
  auto req = new ExecutionRequest();
  unpack_exe_request(req_buff, req);
  printf("get a exe request txnid = %ld\n", req->txn_id);
  auto response = new ExecutionRes();
  response->txn_id = req->txn_id;
  //   get read data
  auto success = server->store->get_read_set(req, response);
  printf("read success = %d, size = %d\n", success, response->read_set.size());
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
  printf("response read set size = %d, success = %d\n",
         response->read_set.size(), response->success);
  // serialize reponse
  erpc::MsgBuffer &resp = req_handle->pre_resp_msgbuf_;
  serialize_exe_response(&resp, response);
  // insert into wait list

  // reply to client
  server->rpc_->enqueue_response(req_handle, &resp);
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

MemServer::MemServer(size_t _thread_id, DataStore *store_) {
  thread_id = _thread_id;
  store = store_;
  // init data store
  // switch (db_type) {
  //   case Micro: {
  //     //
  //     store = new Micro_Db();
  //     break;
  //   }
  //   default: {
  //   }
  // }
}