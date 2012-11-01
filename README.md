
# node-leveled

A js-style LevelDB binding for node.

The datatype is plain strings, support for buffers and other formats may be
added.

## Usage

```javascript
var db = require('leveled')('/tmp/mydb');

db.putSync('foo', 'bar')

db.get('some', function (err, val) {
  val == 'value';
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

### leveled#put(key, val[, cb])
### leveled#putSync(key, val)

### leveled#get(key, cb)
### leveled#getSync(key)

### leveled#del(key[, cb])
### leveled#delSync(key)

### leveled#batch()

### batch#put(key, val)
### batch#del(key)

### batch#write(cb)
### batch#writeSync()

## Benchmark

```bash
$ node bench/bench.js

  benchmarking with 120000 records, 24 chars each

          put :  135746 w/s in   884ms
      putSync :  369230 w/s in   325ms

        batch :  634920 w/s in   189ms
    batchSync :  396039 w/s in   303ms

          get :   58536 r/s in  2050ms
      getSync :  579710 r/s in   207ms

```

## TODO

* evaluate leveldb::Slice for storage
* evaluate buffers as data type
* evaluate storing native js objects
* evaluate msgpack
* implement iterators
  * check less verbose iterator patterns

## License

(MIT)
