
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

Dunno yet why `leveled#get(key, cb)` is so slow...

```bash
$ node bench/bench.js

  benchmarking with 120,000 records, 24 chars each

          put :  137,457 w/s in    873ms
      putSync :  376,175 w/s in    319ms

        batch :  634,920 w/s in    189ms
    batchSync :  674,157 w/s in    178ms

          get :   61,255 r/s in  1,959ms
      getSync :  582,524 r/s in    206ms

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
