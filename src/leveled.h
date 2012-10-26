#ifndef NODE_LEVELED
#define NODE_LEVELED

#include <node.h>
#include <leveldb/db.h>

using namespace v8;
using namespace node;

class Leveled : public ObjectWrap {
public:
  static void Initialize(Handle<Object> target);
  Leveled(char* path);
  ~Leveled();

  static Persistent<FunctionTemplate> constructor;
  static Handle<Value> New(const Arguments &args);

  static Handle<Value> Get(const Arguments &args);
  static Handle<Value> Put(const Arguments &args);

private:
  leveldb::DB *db;
};

#endif
