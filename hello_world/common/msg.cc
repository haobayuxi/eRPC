
#include "msg.h"

int serialize_DataItem(char *buf, struct DataItem *item) {
  memcpy(buf, item->key, KeySize);
  memcpy(buf + KeySize, &item->ts, 8);

  return KeySize + 8 + item->data_size;
}

void unpack_exe_request(const erpc::MsgBuffer *req_msgbuf,
                        ExecutionRequest *request) {
  uint8_t *buf = req_msgbuf->buf_;
  memcpy(&request->txn_id, buf, 8);
  buf += 8;
  int read_set_size = 0;
  memcpy(&read_set_size, buf, 4);
  buf += 4;
  for (int i = 0; i < read_set_size; i++) {
    struct Key key;
    memcpy(&key, buf, KeySize);
    request->read_set.push_back(key);
    buf += KeySize;
  }
  int write_set_size = 0;
  memcpy(&write_set_size, buf, 4);
  buf += 4;
  for (int i = 0; i < write_set_size; i++) {
    struct Key key;
    memcpy(&key, buf, KeySize);
    request->write_set.push_back(key);
    buf += KeySize;
  }
}

void serialize_exe_response(const erpc::MsgBuffer *req_msgbuf,
                            ExecutionRes *response) {
  uint8_t *buf = req_msgbuf->pre_resp_msgbuf_;
  memcpy(&response->txn_id, buf, 8);
  buf += 8;
  int read_set_size = response->read_set.size();
  memcpy(buf, &read_set_size, 4);
  buf += 4;
  for (int i = 0; i < read_set_size; i++) {
    int size =
  }
}

void unpack_exe_response(erpc::MsgBuffer &req_msgbuf, ExecutionRes *response) {}