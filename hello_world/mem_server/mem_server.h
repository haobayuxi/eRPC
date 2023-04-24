
#include "../common/common.h"
#include "../common/msg.h"
#include "../workload/micro_db.h"
#include "rpc.h"

class MemServer {
 public:
  MemServer(size_t _thread_id, Micro_Db *store_);
  int id;
  erpc::Rpc<erpc::CTransport> *rpc_;
  size_t thread_id;
  DbType db_type;

  Micro_Db *store;

  //   txns
  unordered_map<int, struct ExecutionRequest *> txns;

 private:
};

void run_server(MemServer *s, erpc::Nexus *nexus);

void handle_execute(erpc::ReqHandle *req_handle, void *_handler);

void handle_validate(erpc::ReqHandle *req_handle, void *_handler);

void handle_commit(erpc::ReqHandle *req_handle, void *_handler);

void handle_abort(erpc::ReqHandle *req_handle, void *_handler);