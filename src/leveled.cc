#include <node.h>
#include <v8.h>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include "leveled.h"

using namespace v8;
using namespace node;

Persistent<FunctionTemplate> Leveled::constructor;

void Leveled::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  constructor = Persistent<FunctionTemplate>::New(tpl);
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("Leveled"));

  SetPrototypeMethod(constructor, "get", Get);
  SetPrototypeMethod(constructor, "getSync", GetSync);
  SetPrototypeMethod(constructor, "put", Put);
  SetPrototypeMethod(constructor, "putSync", PutSync);

  target->Set(String::NewSymbol("Db"), constructor->GetFunction());
}

Handle<Value> Leveled::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() < 1 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("DB path required")));
  }
  String::Utf8Value name(args[0]);

  Leveled *wrapper = new Leveled(*name);
  wrapper->Wrap(args.Holder());
  return scope.Close(args.Holder());
}

/**
 * Get
 *
 * @param {string} key
 * @param {function} cb
 * @returns {object} Leveled
 */

struct ReadParams {
  Leveled* self;
  std::string key; 
  Persistent<Function> callback;
  leveldb::Status status;
  std::string rtn;
};

Handle<Value> Leveled::Get(const Arguments& args) {
  HandleScope scope;
  Leveled* self = ObjectWrap::Unwrap<Leveled>(args.This());

  if (args.Length() < 2 || !args[1]->IsFunction()) {
    ThrowException(Exception::Error(String::New("key and callback required")));
    return scope.Close(Undefined());
  }

  uv_work_t *req = new uv_work_t;
  ReadParams *params = new ReadParams;
  params->self = self;
  params->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));
  String::Utf8Value key(args[0]->ToString());
  params->key = std::string(*key);
  req->data = params;

  uv_queue_work(uv_default_loop(), req, GetDoing, GetAfter);

  //return scope.Close(args.This());
  return args.This();
}

void Leveled::GetDoing (uv_work_t *req) {
  ReadParams *params = (ReadParams *)req->data;

  std::string value;
  params->self->db->Get(leveldb::ReadOptions(), params->key, &value);
  //params->self->db->Get(leveldb::ReadOptions(), "key", &value);
  params->rtn = value;
}

void Leveled::GetAfter (uv_work_t *req) {
  HandleScope scope;
  ReadParams *params = (ReadParams *)req->data;

  Handle<Value> argv[1];
  argv[0] = Local<Value>::New(Undefined());
  argv[1] = Local<Value>::New(String::New(params->rtn.data()));

  params->callback->Call(Context::GetCurrent()->Global(), 2, argv);

  params->callback.Dispose();
  delete params;
  delete req;
}

/**
 * GetSync
 *
 * @param {string} key
 * @returns {string} result
 */

Handle<Value> Leveled::GetSync(const Arguments& args) {
  HandleScope scope;
  Leveled* self = ObjectWrap::Unwrap<Leveled>(args.This());

  if (args.Length() == 0) {
    ThrowException(Exception::Error(String::New("key required")));
    return scope.Close(Undefined());
  }

  String::Utf8Value key(args[0]->ToString());
  std::string value;
  self->db->Get(leveldb::ReadOptions(), *key, &value);

  return scope.Close(String::New(value.data()));
}

/**
 * Put
 *
 * @param {string} key
 * @param {string} val
 * @param {function=} cb
 * @returns {object} Leveled
 */

Handle<Value> Leveled::Put(const Arguments& args) {
  HandleScope scope;
  Leveled* self = ObjectWrap::Unwrap<Leveled>(args.This());

  if (args.Length() < 2) {
    ThrowException(Exception::Error(String::New("key and value required")));
    return scope.Close(Undefined());
  }

  String::Utf8Value key(args[0]->ToString());
  String::Utf8Value val(args[1]->ToString());

  self->db->Put(leveldb::WriteOptions(), *key, *val);

  if (args.Length() > 2 && args[2]->IsFunction()) {
    Local<Function> cb = Local<Function>::Cast(args[2]);
    const unsigned argc = 1;
    Local<Value> argv[argc] = { Local<Value>::New(Undefined()) };
    cb->Call(Context::GetCurrent()->Global(), argc, argv);
  }

  return scope.Close(args.This());
}

/**
 * PutSync
 *
 * @param {string} key
 * @param {string} val
 * @returns {object} Leveled
 */

Handle<Value> Leveled::PutSync(const Arguments& args) {
  HandleScope scope;
  Leveled* self = ObjectWrap::Unwrap<Leveled>(args.This());

  if (args.Length() < 2) {
    ThrowException(Exception::Error(String::New("Key and Value required")));
    return scope.Close(Undefined());
  }

  String::Utf8Value key(args[0]->ToString());
  String::Utf8Value val(args[1]->ToString());

  leveldb::Status status = self->db->Put(leveldb::WriteOptions(), *key, *val);

  if (!status.ok()) {
    ThrowException(Exception::Error(String::New(status.ToString().data())));
    return scope.Close(Undefined());
  }

  return scope.Close(args.This());
}

/**
 * Leveled constructor
 *
 * Creates the DB to use
 *
 * @param {char*} path
 */

Leveled::Leveled(char* path) {
  HandleScope scope;

  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, path, &db);

  if (!status.ok()) {
    ThrowException(Exception::Error(String::New(status.ToString().data())));
  }
}

Leveled::~Leveled() {
  if (db != NULL) {
    delete db;
    db = NULL;
  }
}

extern "C" void init(Handle<Object> target) {
  Leveled::Initialize(target);
}
NODE_MODULE(leveled, init);
