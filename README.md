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

## TODO

* evaluate `leveldb::Slice` for storage
* evaluate buffers as data type
* evaluate storing native js objects
* evaluate msgpack
* implement iterators
  * check less verbose iterator patterns
* develop middleware api
  * check connect middleware api
  * check other middleware implementations

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
  var write = res.write;

  if (req.method == 'put') req.val = compress(val)
  if (req.method == 'get') {
    res.write = function (data) {
      write(decompress(data))
    }
  }
  next()
})
```

## License

(MIT)
