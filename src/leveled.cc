#include <node.h>
#include <v8.h>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include "leveled.h"
#include "batch.h"

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
  SetPrototypeMethod(constructor, "write", Write);
  SetPrototypeMethod(constructor, "writeSync", WriteSync);

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
 * GetSync
 *
 * @param {string} key
 * @returns {string} result
 */

Handle<Value> Leveled::GetSync(const Arguments& args) {
  HandleScope scope;
  Leveled* self = ObjectWrap::Unwrap<Leveled>(args.Holder());

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
 * Get
 *
 * @param {string} key
 * @param {function} cb
 * @returns {object} Leveled
 */

Handle<Value> Leveled::Get(const Arguments& args) {
  HandleScope scope;
  Leveled* self = ObjectWrap::Unwrap<Leveled>(args.Holder());

  if (args.Length() < 2 || !args[1]->IsFunction()) {
    ThrowException(Exception::Error(String::New("key and cb required")));
    return scope.Close(Undefined());
  }

  uv_work_t *req = new uv_work_t;
  GetParams *params = new GetParams;
  params->self = self;
  params->cb = Persistent<Function>::New(Local<Function>::Cast(args[1]));
  String::Utf8Value key(args[0]->ToString());
  params->key = std::string(*key);
  req->data = params;

  uv_queue_work(uv_default_loop(), req, GetDoing, GetAfter);

  return scope.Close(args.Holder());
}

void Leveled::GetDoing (uv_work_t *req) {
  GetParams *params = (GetParams *)req->data;

  params->status = params->self->db->Get(
    leveldb::ReadOptions(),
    params->key,
    &params->rtn
  );
}

void Leveled::GetAfter (uv_work_t *req) {
  HandleScope scope;
  GetParams *params = (GetParams *)req->data;

  Handle<Value> argv[1];
  
  if (params->status.ok()) {
    argv[0] = Local<Value>::New(Undefined());
  } else {
    argv[0] = Local<Value>::New(String::New(params->status.ToString().data()));
  }

  argv[1] = Local<Value>::New(String::New(params->rtn.data()));

  params->cb->Call(Context::GetCurrent()->Global(), 2, argv);

  params->cb.Dispose();
  delete params;
  delete req;
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
  Leveled* self = ObjectWrap::Unwrap<Leveled>(args.Holder());

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

  return scope.Close(args.Holder());
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
  Leveled* self = ObjectWrap::Unwrap<Leveled>(args.Holder());

  if (args.Length() < 2) {
    ThrowException(Exception::Error(String::New("key and value required")));
    return scope.Close(Undefined());
  }

  uv_work_t *req = new uv_work_t;

  PutParams *params = new PutParams;
  params->self = self;
  params->cb = Persistent<Function>::New(Local<Function>::Cast(args[2]));
  String::Utf8Value key(args[0]->ToString());
  String::Utf8Value val(args[1]->ToString());
  params->key = std::string(*key);
  params->val = std::string(*val);
  
  req->data = params;

  uv_queue_work(uv_default_loop(), req, PutDoing, PutAfter);

  return scope.Close(args.Holder());
}

void Leveled::PutDoing (uv_work_t *req) {
  PutParams *params = (PutParams *)req->data;

  params->status = params->self->db->Put(
    leveldb::WriteOptions(),
    params->key,
    params->val  
  );
}

void Leveled::PutAfter (uv_work_t *req) {
  HandleScope scope;
  PutParams *params = (PutParams *)req->data;

  Handle<Value> argv[0];

  if (params->status.ok()) {
    argv[0] = Local<Value>::New(Undefined());
  } else {
    argv[0] = Local<Value>::New(String::New(params->status.ToString().data()));
  }

  if (params->cb->IsFunction()) {
    params->cb->Call(Context::GetCurrent()->Global(), 1, argv);
  }

  params->cb.Dispose();
  delete params;
  delete req;
}

/**
 * WriteSync
 *
 * @param {object} batch
 * @returns {object} Leveled
 */

Handle<Value> Leveled::WriteSync(const Arguments& args) {
  HandleScope scope;
  Leveled* self = ObjectWrap::Unwrap<Leveled>(args.Holder());

  if (args.Length() < 1) {
    ThrowException(Exception::Error(String::New("batch required")));
    return scope.Close(Undefined());
  }

  Batch* batch = node::ObjectWrap::Unwrap<Batch>(args[0]->ToObject());

  leveldb::Status status = self->db->Write(leveldb::WriteOptions(), &batch->batch);

  if (!status.ok()) {
    ThrowException(Exception::Error(String::New(status.ToString().data())));
    return scope.Close(Undefined());
  }

  return scope.Close(args.Holder());
}

/**
 * Write
 *
 * @param {object} batch
 * @param {function=} cb
 * @returns {object} Leveled
 */

Handle<Value> Leveled::Write(const Arguments& args) {
  HandleScope scope;
  Leveled* self = ObjectWrap::Unwrap<Leveled>(args.Holder());

  if (args.Length() < 1) {
    ThrowException(Exception::Error(String::New("batch required")));
    return scope.Close(Undefined());
  }

  uv_work_t *req = new uv_work_t;

  WriteParams *params = new WriteParams;
  params->self = self;
  params->cb = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  Batch* batch = node::ObjectWrap::Unwrap<Batch>(args[0]->ToObject());
  params->batch = batch;
  
  req->data = params;

  uv_queue_work(uv_default_loop(), req, WriteDoing, WriteAfter);

  return scope.Close(args.Holder());
}

void Leveled::WriteDoing (uv_work_t *req) {
  WriteParams *params = (WriteParams *)req->data;

  params->status = params->self->db->Write(
    leveldb::WriteOptions(),
    &params->batch->batch
  );
}

void Leveled::WriteAfter (uv_work_t *req) {
  HandleScope scope;
  WriteParams *params = (WriteParams *)req->data;

  Handle<Value> argv[0];

  if (params->status.ok()) {
    argv[0] = Local<Value>::New(Undefined());
  } else {
    argv[0] = Local<Value>::New(String::New(params->status.ToString().data()));
  }

  if (params->cb->IsFunction()) {
    params->cb->Call(Context::GetCurrent()->Global(), 1, argv);
  }

  params->cb.Dispose();
  delete params;
  delete req;
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
  Batch::Initialize(target);
}
NODE_MODULE(leveled, init);
