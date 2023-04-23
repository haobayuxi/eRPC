
#include "../common.h"
#include "rpc.h"

class MemServer {
 public:
  int id;
  erpc::Rpc<erpc::CTransport>* rpc_;
  size_t thread_id;

 private:
};

void run_server(MemServer* s, erpc::Nexus* nexus);