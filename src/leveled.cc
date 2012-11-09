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
  SetPrototypeMethod(constructor, "find", Find);
  SetPrototypeMethod(constructor, "range", Range);
  SetPrototypeMethod(constructor, "put", Put);
  SetPrototypeMethod(constructor, "putSync", PutSync);
  SetPrototypeMethod(constructor, "write", Write);
  SetPrototypeMethod(constructor, "writeSync", WriteSync);
  SetPrototypeMethod(constructor, "del", Del);
  SetPrototypeMethod(constructor, "delSync", DelSync);

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
  leveldb::Status status = self->db->Get(leveldb::ReadOptions(), *key, &value);

  if (!status.ok()) {
    ThrowException(Exception::Error(String::New(status.ToString().data())));
  }

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

  Handle<Value> argv[2];
  
  if (params->status.ok()) {
    argv[0] = Local<Value>::New(Undefined());
  } else {
    argv[0] = Local<Value>::New(String::New(params->status.ToString().data()));
  }

  argv[1] = Local<Value>::New(String::New(params->rtn.data()));

  TryCatch try_catch;
  params->cb->Call(Context::GetCurrent()->Global(), 2, argv);
  if (try_catch.HasCaught()) {
    FatalException(try_catch);
  }

  params->cb.Dispose();
  delete params;
  delete req;
  scope.Close(Undefined());
}

/**
 * Find
 *
 * @param {string} glob
 * @param {function} cb
 * @returns {object} Leveled
 */

Handle<Value> Leveled::Find(const Arguments& args) {
  HandleScope scope;
  Leveled* self = ObjectWrap::Unwrap<Leveled>(args.Holder());

  if (args.Length() < 2 || !args[1]->IsFunction()) {
    ThrowException(Exception::Error(String::New("glob and cb required")));
    return scope.Close(Undefined());
  }

  uv_work_t *req = new uv_work_t;
  FindParams *params = new FindParams;
  params->self = self;
  params->cb = Persistent<Function>::New(Local<Function>::Cast(args[1]));
  String::Utf8Value glob(args[0]->ToString());
  params->glob = std::string(*glob);
  req->data = params;

  uv_queue_work(uv_default_loop(), req, FindDoing, FindAfter);

  return scope.Close(args.Holder());
}

void Leveled::FindDoing (uv_work_t *req) {
  FindParams *params = (FindParams *)req->data;

  leveldb::Iterator* it = params->self->db->NewIterator(leveldb::ReadOptions());

  const char last = params->glob[params->glob.length() - 1];
  char next;
  if (last == '*') {
    next = '{';
  } else {
    next = last + 1;
  }

  std::string end = params->glob.substr(0, params->glob.length() - 1) + next;

  for (it->Seek(params->glob); it->Valid() && it->key().ToString() < end; it->Next()) {
    params->rtn[it->key().ToString()] = it->value().ToString();
  }
  params->status = it->status();
  delete it;
}

void Leveled::FindAfter (uv_work_t *req) {
  HandleScope scope;
  FindParams *params = (FindParams *)req->data;

  Handle<Value> argv[2];

  if (params->status.ok()) {
    argv[0] = Local<Value>::New(Undefined());
  } else {
    argv[0] = Local<Value>::New(String::New(params->status.ToString().data()));
  }

  Local<Object> rtn = Object::New();
  std::map<std::string, std::string> map(params->rtn);
  std::map<std::string, std::string>::iterator it;
  for (it = map.begin(); it != map.end(); it++) {
    rtn->Set(
      String::New(it->first.data()),
      String::New(it->second.data())
    );
  }
  argv[1] = rtn;

  TryCatch try_catch;
  params->cb->Call(Context::GetCurrent()->Global(), 2, argv);
  if (try_catch.HasCaught()) FatalException(try_catch);

  params->cb.Dispose();
  delete params;
  delete req;

  scope.Close(Undefined());
}

/**
 * Range
 *
 * @param {string} from
 * @param {string} to
 * @param {function} cb
 * @returns {object} Leveled
 */

Handle<Value> Leveled::Range(const Arguments& args) {
  HandleScope scope;
  Leveled* self = ObjectWrap::Unwrap<Leveled>(args.Holder());

  if (args.Length() < 3 || !args[2]->IsFunction()) {
    ThrowException(Exception::Error(String::New("from, to and cb required")));
    return scope.Close(Undefined());
  }

  uv_work_t *req = new uv_work_t;
  RangeParams *params = new RangeParams;
  params->self = self;
  params->cb = Persistent<Function>::New(Local<Function>::Cast(args[2]));
  String::Utf8Value from(args[0]->ToString());
  params->from = std::string(*from);
  String::Utf8Value to(args[1]->ToString());
  params->to = std::string(*to);
  req->data = params;

  uv_queue_work(uv_default_loop(), req, RangeDoing, RangeAfter);

  return scope.Close(args.Holder());
}

void Leveled::RangeDoing (uv_work_t *req) {
  RangeParams *params = (RangeParams *)req->data;

  leveldb::Iterator* it = params->self->db->NewIterator(leveldb::ReadOptions());

  std::string from = params->from;
  std::string to = params->to;
  bool skipFirst = false;
  bool skipLast = false;
  int lastTo = to.length() - 1;

  if (from[0] == '[' || from[0] == '(') from = from.substr(1, from.length() - 1);
  if (to[lastTo] == ']' || to[lastTo] == ')') to = to.substr(0, to.length() - 1);
  if (params->from[0] == '(') skipFirst = true;
  if (params->to[lastTo] == ')') skipLast = true;

  std::string key;

  for (it->Seek(from); it->Valid() && it->key().ToString() <= to; it->Next()) {
    key = it->key().ToString();
    if (key == from && skipFirst) continue;
    if (key == to && skipLast) continue;
    params->rtn[key] = it->value().ToString();
  }

  params->status = it->status();
  delete it;
}

void Leveled::RangeAfter (uv_work_t *req) {
  HandleScope scope;
  RangeParams *params = (RangeParams *)req->data;

  Handle<Value> argv[2];

  if (params->status.ok()) {
    argv[0] = Local<Value>::New(Undefined());
  } else {
    argv[0] = Local<Value>::New(String::New(params->status.ToString().data()));
  }

  Local<Object> rtn = Object::New();
  std::map<std::string, std::string> map(params->rtn);
  std::map<std::string, std::string>::iterator it;
  for (it = map.begin(); it != map.end(); it++) {
    rtn->Set(
      String::New(it->first.data()),
      String::New(it->second.data())
    );
  }
  argv[1] = rtn;

  TryCatch try_catch;
  params->cb->Call(Context::GetCurrent()->Global(), 2, argv);
  if (try_catch.HasCaught()) FatalException(try_catch);

  params->cb.Dispose();
  delete params;
  delete req;

  scope.Close(Undefined());
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
  if (!args[1]->IsString()) {
    ThrowException(Exception::Error(String::New("Only strings as values")));
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
  if (!args[1]->IsString()) {
    ThrowException(Exception::Error(String::New("Only strings as values")));
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

  Handle<Value> argv[1];

  if (params->status.ok()) {
    argv[0] = Local<Value>::New(Undefined());
  } else {
    argv[0] = Local<Value>::New(String::New(params->status.ToString().data()));
  }

  if (params->cb->IsFunction()) {
    TryCatch try_catch;
    params->cb->Call(Context::GetCurrent()->Global(), 1, argv);
    if (try_catch.HasCaught()) {
      FatalException(try_catch);
    }
  }

  params->cb.Dispose();
  delete params;
  delete req;
  scope.Close(Undefined());
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

  leveldb::Status status = self->db->Write(
    leveldb::WriteOptions(),
    &batch->batch
  );

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

  WriteParams *params = new WriteParams();
  params->request.data = params;
  params->db = self->db;
  params->cb = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  params->batch = ObjectWrap::Unwrap<Batch>(args[0]->ToObject());

  uv_queue_work(uv_default_loop(), &params->request, WriteDoing, WriteAfter);

  return scope.Close(args.Holder());
}

void Leveled::WriteDoing (uv_work_t *req) {
  WriteParams *params = (WriteParams *)req->data;
  leveldb::WriteBatch wb = params->batch->batch;

  params->status = params->db->Write(leveldb::WriteOptions(), &wb);
}

void Leveled::WriteAfter (uv_work_t *req) {
  HandleScope scope;
  WriteParams *params = (WriteParams *)req->data;

  Handle<Value> argv[1];

  if (params->status.ok()) {
    argv[0] = Local<Value>::New(Undefined());
  } else {
    argv[0] = Local<Value>::New(String::New(params->status.ToString().data()));
  }

  if (params->cb->IsFunction()) {
    TryCatch try_catch;
    params->cb->Call(Context::GetCurrent()->Global(), 1, argv);
    if (try_catch.HasCaught()) {
      FatalException(try_catch);
    }
  }

  params->cb.Dispose();
  delete params;
  scope.Close(Undefined());
}

/**
 * DelSync
 *
 * @param {string} key
 * @returns {Object} Leveled
 */

Handle<Value> Leveled::DelSync(const Arguments& args) {
  HandleScope scope;
  Leveled* self = ObjectWrap::Unwrap<Leveled>(args.Holder());

  if (args.Length() == 0) {
    ThrowException(Exception::Error(String::New("key required")));
    return scope.Close(Undefined());
  }

  String::Utf8Value key(args[0]->ToString());
  leveldb::Status status = self->db->Delete(leveldb::WriteOptions(), *key);

  if (!status.ok()) {
    ThrowException(Exception::Error(String::New(status.ToString().data())));
  }

  return scope.Close(args.Holder());
}

/**
 * Del
 *
 * @param {string} key
 * @param {function} cb
 * @returns {object} Leveled
 */

Handle<Value> Leveled::Del(const Arguments& args) {
  HandleScope scope;
  Leveled* self = ObjectWrap::Unwrap<Leveled>(args.Holder());

  if (args.Length() < 2 || !args[1]->IsFunction()) {
    ThrowException(Exception::Error(String::New("key and cb required")));
    return scope.Close(Undefined());
  }

  uv_work_t *req = new uv_work_t;
  DelParams *params = new DelParams;
  params->self = self;
  params->cb = Persistent<Function>::New(Local<Function>::Cast(args[1]));
  String::Utf8Value key(args[0]->ToString());
  params->key = std::string(*key);
  req->data = params;

  uv_queue_work(uv_default_loop(), req, DelDoing, DelAfter);

  return scope.Close(args.Holder());
}

void Leveled::DelDoing (uv_work_t *req) {
  DelParams *params = (DelParams *)req->data;

  params->status = params->self->db->Delete(
    leveldb::WriteOptions(),
    params->key
  );
}

void Leveled::DelAfter (uv_work_t *req) {
  HandleScope scope;
  DelParams *params = (DelParams *)req->data;

  Handle<Value> argv[1];
  
  if (params->status.ok()) {
    argv[0] = Local<Value>::New(Undefined());
  } else {
    argv[0] = Local<Value>::New(String::New(params->status.ToString().data()));
  }

  TryCatch try_catch;
  params->cb->Call(Context::GetCurrent()->Global(), 1, argv);
  if (try_catch.HasCaught()) {
    FatalException(try_catch);
  }

  params->cb.Dispose();
  delete params;
  delete req;
  scope.Close(Undefined());
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
