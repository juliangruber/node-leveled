# node-leveled

A js-style LevelDB binding for node.

**_DEPRECATED - Use [level](https://github.com/Level/level)_**

[![Build Status](https://travis-ci.org/juliangruber/node-leveled.png?branch=master)](https://travis-ci.org/juliangruber/node-leveled)

The datatype is plain strings, support for buffers and other formats may be
added.

## Usage

```javascript
var db = require('leveled')('/tmp/mydb');

db.putSync('foo', 'bar')

db.get('some', function (err, val) {
  val == 'value';
})

db.find('*', function (err, res) {
  console.log(res); // { 'a' : 'foo', 'aa' : 'baz', ... }  
})

db.batch()
  .put('foo', 'bar')
  .del('baz')
  .write(function (err) {
    // success
  })
```

## Installation

```bash
$ npm install leveled
```

## API

### leveled(path)

Instantiates a new DB at `path`, creating `path` if necessary.

### leveled#put(key, val[, cb])
### leveled#putSync(key, val)

Store `val` at `key`.

### leveled#get(key, cb)
### leveled#getSync(key)

Get the value stored at `key`.

### leveled#del(key[, cb])
### leveled#delSync(key)

Delete the value stored at `key`.

### leveled#range(from, to, cb)

Find all entries whose keys are in the give range.

* `from` can be `from` / `[from` or `(from`
* `to` can be `to` / `to]` or `to)`

```js
db.range('[1337', '2337)', function (err, res) {
  console.log(res) // { 1337 : 'foo', ..., 2336 : 'foo' }  
})
```

### leveled#find(glob, cb)

Find values.

```js
db.find('ab*', function (err, res) {
  console.log(res) // { 'aba' : 'foo', 'abzzz' : 'bar' }
})
```

At the moment glob-style matching is not fully implemented, what works is

* `abc*`
* `*`

In the future it will work like redis's `KEYS` command:

* `ab?` matches `abc`, not `abcc`
* `ab[1-3]` matches `ab1`, `ab2`, `ab3`
* `ab[1-3]*` etc.

### leveled#batch()

Creates a new `batch` that queues up operations until its `write` method is invoked.

### batch#put(key, val)
### batch#del(key)

### batch#write(cb)
### batch#writeSync()

Apply the batch's operations to the DB.

## Benchmark

On my mb pro:

```bash
$ node bench/bench.js

  benchmarking with 120,000 records, 24 chars each

          put :   128,479 w/s in    934ms
      putSync :   372,670 w/s in    322ms

        batch :   612,244 w/s in    196ms
    batchSync :   641,711 w/s in    187ms

          get :    58,881 r/s in  2,038ms
      getSync :   560,747 r/s in    214ms

     iterator :   220,588 r/s in    544ms

```

`put` oparations don't force a sync to disk, hence the `get` looking so slow. Iterators are faster for common tasks where you need to get many values anyways.

## TODO

* evaluate leveldb::Slice for storage
* evaluate buffers as data type
* evaluate storing native js objects
* evaluate msgpack

## License

(MIT)
