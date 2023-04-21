#include <stdio.h>

#include "rpc.h"

static const std::string kServerHostname = "192.168.3.71";
static const std::string kClientHostname = "192.168.3.72";

static constexpr uint16_t kUDPPort = 31850;
static constexpr uint8_t kReqType = 2;
static constexpr size_t kMsgSize = 16;

class BasicAppContext {
 public:
  //   TmpStat *tmp_stat_ = nullptr;
  erpc::Rpc<erpc::CTransport> *rpc_ = nullptr;
  erpc::FastRand fastrand_;

  std::vector<int> session_num_vec_;

  size_t thread_id_;           // The ID of the thread that owns this context
  size_t num_sm_resps_ = 0;    // Number of SM responses
  bool ping_pending_ = false;  // Only one ping is allowed at a time

  ~BasicAppContext() {
    // if (tmp_stat_ != nullptr) delete tmp_stat_;
  }

  // Use Lemire's trick to get a random session number from session_num_vec
  inline int fast_get_rand_session_num() {
    uint32_t x = fastrand_.next_u32();
    size_t rand_index =
        (static_cast<size_t>(x) * session_num_vec_.size()) >> 32;
    return session_num_vec_[rand_index];
  }
};