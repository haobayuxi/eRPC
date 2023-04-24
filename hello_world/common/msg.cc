
#include "msg.h"

void serialize_exe_request(const erpc::MsgBuffer *req_msgbuf,
                           vector<DataItemPtr> *read_set,
                           vector<DataItemPtr> *write_set, uint64_t txn_id) {
  // resize

  uint8_t *buf = req_msgbuf->buf_;
  memcpy(buf, &txn_id, 8);
  buf += 8;
  int read_set_size = read_set->size();
  memcpy(buf, &read_set_size, 4);
  buf += 4;
  for (int i = 0; i < read_set_size; i++) {
    memcpy(buf, read_set->at(i), KeySize);
    buf += KeySize;
  }
  int write_set_size = write_set->size();
  memcpy(buf, &write_set_size, 4);
  buf += 4;
  for (int i = 0; i < write_set_size; i++) {
    memcpy(buf, write_set->at(i), KeySize);
    buf += KeySize;
  }
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
  uint8_t *buf = req_msgbuf->buf_;
  memcpy(buf, &response->txn_id, 8);
  buf += 8;
  int read_set_size = response->read_set.size();
  memcpy(buf, &read_set_size, 4);
  buf += 4;
  for (int i = 0; i < read_set_size; i++) {
    memcpy(buf, &response->read_set[i], DataItemSize);
    buf += DataItemSize;
  }
  memcpy(buf, &response->success, 1);
}

void unpack_exe_response(erpc::MsgBuffer *req_msgbuf, ExecutionRes *response) {
  uint8_t *buf = req_msgbuf->buf_;
  memcpy(&response->txn_id, buf, 8);
  buf += 8;
  int read_set_size = 0;
  memcpy(&read_set_size, buf, 4);
  buf += 4;
  for (int i = 0; i < read_set_size; i++) {
    struct DataItem item;
    memcpy(&item, buf, DataItemSize);
    response->read_set.push_back(item);
    buf += DataItemSize;
  }
  memcpy(&response->success, buf, 1);
}

void unpack_validate_request(erpc::MsgBuffer &req_msgbuf,
                             ValidationRequest *request) {}