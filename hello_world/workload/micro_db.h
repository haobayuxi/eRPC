#include <string.h>

#include <unordered_map>

#include "tuple_meta.h"

using namespace std;
class Micro_Db {
 public:
  unordered_map<uint64_t, TupleMeta> data;
};
