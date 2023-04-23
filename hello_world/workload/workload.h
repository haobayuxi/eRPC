
#include <iostream>

using namespace std;

class DataStore {
 public:
  void tfunc() {}
};

class Test : public DataStore {
 public:
  virtual void tfunc() { cout << "this is test" << endl; }
};

class Test1 : public DataStore {
 public:
  virtual void tfunc() { cout << "this is test1" << endl; }
};