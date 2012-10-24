#ifndef NODE_LEVELED
#define NODE_LEVELED

#include <node.h>
#include <leveldb/db.h>

using namespace v8;
using namespace node;

class Leveled : ObjectWrap {
public:
    Leveled(const Arguments &args);
    ~Leveled();

    static void Initialize(Handle<Object> target);

    static Handle<Value> get(const Arguments &args);
    static Handle<Value> set(const Arguments &args);

private:
    leveldb::db *db;
}
