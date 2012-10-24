#include <node.h>
#include <v8.h>

using namespace v8;

Handle<Value> DB (const Arguments& args) {
  HandleScope scope;

  if (args.Length() < 1) {
    ThrowException(Exception::TypeError(String::New("DB path required")));
    return scope.Close(Undefined());
  }

  return scope.Close(args.This());
}

Handle<Value> get (const Arguments &args) {
  HandleScope scope;

  //Local<String> key = args[0];
  Local<Function> cb = Local<Function>::Cast(args[1]);
  const unsigned argc = 1;
  Local<Value> argv[argc] = { Local<Value>::New(String::New("ha!")) };
  cb->Call(Context::GetCurrent()->Global(), argc, argv);

  return scope.Close(args.This());
}

void init (Handle<Object> target) {
  HandleScope scope;

  Handle<FunctionTemplate> dbTpl = FunctionTemplate::New(DB);

  //Handle<ObjectTemplate> instance = dbTpl->InstanceTemplate();

  NODE_SET_PROTOTYPE_METHOD(dbTpl, "get", get);
  //NODE_SET_PROTOTYPE_METHOD(dbTpl, "set", set);

  //NODE_SET_METHOD(target, "leveled", dbTpl);
  target->Set(
    String::NewSymbol("leveled"),
    dbTpl->GetFunction()    
  );
}

NODE_MODULE(leveled, init)
