#include <node.h>
#include <v8.h>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include "leveled.h"

using namespace v8;
using namespace node;

void Leveled::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Leveled"));

  //sft = Persistent<FunctionTemplate>::New(tpl);
  //sft->InstanceTemplate()->SetInternalFieldCount(1);
  //sft->SetClassName(String::NewSymbol("Leveled"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "get", Get);
  NODE_SET_PROTOTYPE_METHOD(tpl, "set", Set);

  target->Set(String::NewSymbol("Db"), tpl->GetFunction());
}

Handle<Value> Leveled::New(const Arguments& args) {
  HandleScope scope;

  Leveled *wrapper = new Leveled();
  //Leveled *wrapper = new Leveled(args[0]);
  wrapper->Wrap(args.Holder());
  return scope.Close(args.Holder());
}

Handle<Value> Leveled::Get(const Arguments& args) {
  return args.This();
}

Handle<Value> Leveled::Set(const Arguments& args) {
  return args.This();
}

Leveled::Leveled() {
  HandleScope scope;

  /*if (args.Length() < 1) {
    return ThrowException(Exception::Error(String::New("DB path required")));
    }*/

  leveldb::Options options;
  //options.create_if_missing = true;
  //leveldb::Status status = leveldb::DB::Open(options, "/tmp/foo", &db);
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
