#pragma once
#include <stdio.h>

#include "rpc.h"

#define Max_Msg_Size 1000

static const std::string kServerHostname = "192.168.3.71";
static const std::string kClientHostname = "192.168.3.72";

static constexpr uint16_t kUDPPort = 31850;
static constexpr uint8_t kReqType = 2;
static constexpr size_t kMsgSize = 20;
const size_t MAX_ITEM_SIZE = 40;

#define ALWAYS_INLINE inline __attribute__((always_inline))

struct RemoteNode {
  std::string ip;
  uint16_t port;
};

enum DbType {
  Micro = 1,
  Tatp,
};

enum SystemType {
  OCC = 1,
  Meerkat,
  Yuxi,
};

enum TxnStatus {
  Execution = 0,
  Validation,
  Commit,
};