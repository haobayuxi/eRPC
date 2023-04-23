#include <stdio.h>

#include "rpc.h"

#define Max_Msg_Size 100000

static const std::string kServerHostname = "192.168.3.71";
static const std::string kClientHostname = "192.168.3.72";

static constexpr uint16_t kUDPPort = 31850;
static constexpr uint8_t kReqType = 2;
static constexpr size_t kMsgSize = 16;

struct RemoteNode {
  std::string ip;
  uint16_t port;
};