# node-leveled

A js-style LevelDB binding for node.

The datatype is plain strings, support for buffers and other formats may be
added.

## Usage

```javascript
var db = require('leveled')('/tmp/mydb');

db.put('foo', 'bar')

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

### leveled#get(key, cb)

### leveled#del(key[, cb])

### leveled#batch()

### batch#put(key, val)
### batch#del(key)

### batch#write(cb)

## Benchmark

Dunno yet why `leveled#get(key, cb)` is so slow...

```bash
$ node bench/bench.js

  benchmarking with 120,000 records, 24 chars each

          put :  122,699 w/s in    978ms
        batch :  628,272 w/s in    191ms
          get :   58,968 r/s in  2,035ms

```

## TODO

* evaluate leveldb::Slice for storage
* evaluate buffers as data type
* evaluate storing native js objects
* evaluate msgpack
* implement iterators
  * check less verbose iterator patterns

```javascript
// use queue.js for put operations
leveled.use(function (req, res, next) {
  if (req.method != 'put') return next()
  queue.put(res, res)
})
```

```javascript
// always use uppercase keys
leveled.use(function (req, res, next) {
  req.key = req.key.toUppercase()
  next()
})
```

```javascript
// compress data
leveled.use(function (req, res, next) {
  var end = res.end;

  if (req.method == 'put') req.val = compress(req.val)
  if (req.method == 'get') {
    res.end = function (data) {
      end(decompress(data))
    }
  }
  next()
})
```

## License

(MIT)
