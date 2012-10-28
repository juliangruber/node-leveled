#include "batch.h"

using namespace v8;
using namespace node;

Persistent<FunctionTemplate> Batch::constructor;

void Batch::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  constructor = Persistent<FunctionTemplate>::New(tpl);
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("Batch"));

  SetPrototypeMethod(constructor, "put", Put);
  SetPrototypeMethod(constructor, "del", Del);
  SetPrototypeMethod(constructor, "clear", Clear);

  target->Set(String::NewSymbol("Batch"), constructor->GetFunction());
}

Handle<Value> Batch::New(const Arguments& args) {
  HandleScope scope;

  Batch *wrapper = new Batch();
  wrapper->Wrap(args.Holder());
  return scope.Close(args.Holder());
}

/**
 * Put
 *
 * @param {string} key
 * @param {string} val
 * @returns {object} Batch
 */

Handle<Value> Batch::Put(const Arguments& args) {
  HandleScope scope;
  Batch* self = ObjectWrap::Unwrap<Batch>(args.Holder());

  if (args.Length() < 2) {
    ThrowException(Exception::Error(String::New("Key and Value required")));
    return scope.Close(Undefined());
  }

  String::Utf8Value key(args[0]->ToString());
  String::Utf8Value val(args[1]->ToString());

  self->batch.Put(*key, *val);

  return scope.Close(args.Holder());
}

/**
 * Delete
 *
 * @param {string} key
 * @returns {object} Batch
 */

Handle<Value> Batch::Del(const Arguments& args) {
  HandleScope scope;
  Batch* self = ObjectWrap::Unwrap<Batch>(args.Holder());

  if (args.Length() == 0) {
    ThrowException(Exception::Error(String::New("key required")));
    return scope.Close(Undefined());
  }

  String::Utf8Value key(args[0]->ToString());
  self->batch.Delete(*key);

  return scope.Close(args.Holder());
}

/**
 * Clear
 *
 * @returns {object} Batch
 */

Handle<Value> Batch::Clear(const Arguments& args) {
  HandleScope scope;
  Batch* self = ObjectWrap::Unwrap<Batch>(args.Holder());

  self->batch.Clear();

  return scope.Close(args.Holder());
}

/**
 * Batch constructor
 */

Batch::Batch() {}

Batch::~Batch() {}
