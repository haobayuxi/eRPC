
#include "../common.h"
#include "rpc.h"

class MemServer {
 public:
  int id;
  erpc::Rpc<erpc::CTransport>* rpc_;

 private:
}

void run_server(MemServer * s, erpc::Nexus *nexus);