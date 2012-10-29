#ifndef BATCH
#define BATCH

#include <node.h>
#include <leveldb/write_batch.h>

using namespace v8;
using namespace node;

class Batch : public ObjectWrap {
public:
  static void Initialize(Handle<Object> target);

  static Persistent<FunctionTemplate> constructor;
  static Handle<Value> New(const Arguments &args);

  static Handle<Value> Put(const Arguments &args);
  static Handle<Value> Del(const Arguments &args);
  static Handle<Value> Clear(const Arguments &args);

  leveldb::WriteBatch batch;
private:
  friend class Leveled;
  Batch();
  ~Batch();
};

#endif
