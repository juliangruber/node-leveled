#ifndef NODE_LEVELED
#define NODE_LEVELED

#include <node.h>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include "batch.h"

using namespace v8;
using namespace node;

class Leveled : public ObjectWrap {
public:
  static void Initialize(Handle<Object> target);
  Leveled(char* path);
  ~Leveled();

  static Persistent<FunctionTemplate> constructor;
  static Handle<Value> New(const Arguments &args);

  static Handle<Value> GetSync(const Arguments &args);
  static Handle<Value> Get(const Arguments &args);
  static void GetDoing(uv_work_t *req);
  static void GetAfter(uv_work_t *req);

  static Handle<Value> PutSync(const Arguments &args);
  static Handle<Value> Put(const Arguments &args);
  static void PutDoing(uv_work_t *req);
  static void PutAfter(uv_work_t *req);

  static Handle<Value> WriteSync(const Arguments &args);
  static Handle<Value> Write(const Arguments &args);
  static void WriteDoing(uv_work_t *req);
  static void WriteAfter(uv_work_t *req);

private:
  leveldb::DB *db;
};

struct GetParams {
  Leveled* self;
  std::string key; 
  Persistent<Function> cb;
  leveldb::Status status;
  std::string rtn;
};

struct PutParams {
  Leveled* self;
  std::string key; 
  std::string val; 
  Persistent<Function> cb;
  leveldb::Status status;
};

struct WriteParams {
  Leveled* self;
  Batch* batch; 
  Persistent<Function> cb;
  leveldb::Status status;
};

#endif
