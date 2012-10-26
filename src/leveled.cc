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

  NODE_SET_PROTOTYPE_METHOD(constructor, "get", Get);
  NODE_SET_PROTOTYPE_METHOD(constructor, "set", Set);

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

Handle<Value> Leveled::Get(const Arguments& args) {
  HandleScope scope;
  Leveled* self = ObjectWrap::Unwrap<Leveled>(args.This());

  String::Utf8Value key(args[0]->ToString());
  Local<Function> cb = Local<Function>::Cast(args[1]);
  const unsigned argc = 1;

  std::string value;
  self->db->Get(leveldb::ReadOptions(), *key, &value);

  Local<Value> argv[argc] = { Local<Value>::New(String::New(value.data())) };
  cb->Call(Context::GetCurrent()->Global(), argc, argv);

  return scope.Close(Undefined());
}

Handle<Value> Leveled::Set(const Arguments& args) {
  HandleScope scope;
  Leveled* self = ObjectWrap::Unwrap<Leveled>(args.This());

  String::Utf8Value key(args[0]->ToString());
  String::Utf8Value val(args[1]->ToString());

  self->db->Put(leveldb::WriteOptions(), *key, *val);

  return scope.Close(args.This());
}

Leveled::Leveled(char* path) {
  HandleScope scope;

  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, path, &db);
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
