
# node-leveled

A js-style LevelDB binding for node.

## Features

* sync and async methods, except for the constructor, which opens the db
  syncronously
* `put/del` operations are batched automatically since _LevelDB_ only supports
  one write at a time. That makes inserting way faster (see benchmarks)
* `get` operations are always concurrent
* the datatype is plain strings, support for buffers and other formats may be
  added

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

## License

(MIT)
