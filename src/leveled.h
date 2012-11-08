#ifndef NODE_LEVELED
#define NODE_LEVELED

#include <node.h>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include <map>
#include "batch.h"

using namespace v8;
using namespace node;

class Leveled : public ObjectWrap {
public:
  static void Initialize(Handle<Object> target);

private:
  Leveled(char* path);
  ~Leveled();

  static Persistent<FunctionTemplate> constructor;
  leveldb::DB *db;

  static Handle<Value> New(const Arguments &args);

  static Handle<Value> GetSync(const Arguments &args);
  static Handle<Value> Get(const Arguments &args);
  static void GetDoing(uv_work_t *req);
  static void GetAfter(uv_work_t *req);

  static Handle<Value> Find(const Arguments &args);
  static void FindDoing(uv_work_t *req);
  static void FindAfter(uv_work_t *req);

  static Handle<Value> PutSync(const Arguments &args);
  static Handle<Value> Put(const Arguments &args);
  static void PutDoing(uv_work_t *req);
  static void PutAfter(uv_work_t *req);

  static Handle<Value> WriteSync(const Arguments &args);
  static Handle<Value> Write(const Arguments &args);
  static void WriteDoing(uv_work_t *req);
  static void WriteAfter(uv_work_t *req);

  static Handle<Value> DelSync(const Arguments &args);
  static Handle<Value> Del(const Arguments &args);
  static void DelDoing(uv_work_t *req);
  static void DelAfter(uv_work_t *req);
};

struct GetParams {
  Leveled* self;
  std::string key; 
  Persistent<Function> cb;
  leveldb::Status status;
  std::string rtn;
};

struct FindParams {
  Leveled* self;
  std::string glob; 
  Persistent<Function> cb;
  leveldb::Status status;
  std::map<std::string, std::string> rtn;
};

struct PutParams {
  Leveled* self;
  std::string key; 
  std::string val; 
  Persistent<Function> cb;
  leveldb::Status status;
};

struct WriteParams {
  leveldb::DB* db;
  Batch* batch; 
  Persistent<Function> cb;
  leveldb::Status status;
  uv_work_t request;
};

struct DelParams {
  Leveled* self;
  std::string key; 
  Persistent<Function> cb;
  leveldb::Status status;
};

#endif
