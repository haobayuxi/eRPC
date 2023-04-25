
#include "msg.h"

// int serialize_DataItem(uint8_t *buf, DataItem *item) {
//   int size = 0;
//   memcpy(buf, &item->key.table_id, 4);
//   size += 4;
//   memcpy(buf + size, &item->key.key, 8);
//   size += 8;
//   memcpy(buf + size, &item->ts, 8);
//   size += 8;
//   memcpy(buf + size, item->value, 40);
//   size += 40;
//   return size;
// }

// void deserialize_DataItem(uint8_t *buf, DataItem *item) {
//   int size = 0;
//   memcpy(&item->key.table_id, buf, 4);
//   size += 4;
//   memcpy(&item->key.key, buf + size, 8);
//   size += 8;
//   memcpy(&item->ts, buf + size, 8);
//   size += 8;
//   memcpy(item->value, buf + size, 40);
// }

void serialize_exe_request(erpc::MsgBuffer *req_msgbuf,
                           vector<DataItem *> *read_set,
                           vector<DataItem *> *write_set, uint64_t txn_id) {
  // resize

  uint8_t *buf = req_msgbuf->buf_;
  memcpy(buf, &txn_id, 8);
  buf += 8;
  int read_set_size = read_set->size();
  memcpy(buf, &read_set_size, 4);
  buf += 4;
  for (int i = 0; i < read_set_size; i++) {
    memcpy(buf, &read_set->at(i)->key.table_id, 4);
    memcpy(buf + 4, &read_set->at(i)->key.key, 8);
    // printf("serialize key = %ld\n", read_set->at(i)->key.key);
    // struct Key key;
    // memcpy(&key.table_id, buf, 4);
    // memcpy(&key.key, buf + 4, 8);
    // printf("serialize key = %ld\n", key.key);
    buf += KeySize;
  }
  int write_set_size = write_set->size();
  memcpy(buf, &write_set_size, 4);
  buf += 4;
  for (int i = 0; i < write_set_size; i++) {
    memcpy(buf, &write_set->at(i)->key, KeySize);
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
    memcpy(&key.table_id, buf, 4);
    memcpy(&key.key, buf + 4, 8);
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

void serialize_exe_response(erpc::MsgBuffer *req_msgbuf,
                            ExecutionRes *response) {
  uint8_t *buf = req_msgbuf->buf_;
  memcpy(buf, &response->txn_id, 8);
  buf += 8;
  int read_set_size = response->read_set.size();
  memcpy(buf, &read_set_size, 4);
  buf += 4;
  for (int i = 0; i < read_set_size; i++) {
    // auto size = serialize_DataItem(buf, &response->read_set[i]);
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
    // printf("data item key = %ld\n", item.key.key);
    response->read_set.push_back(item);
    buf += DataItemSize;
  }
  memcpy(&response->success, buf, 1);
}

void unpack_validate_request(erpc::MsgBuffer &req_msgbuf,
                             ValidationRequest *request) {}