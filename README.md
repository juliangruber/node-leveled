
# node-leveled

LevelDB

The stored data must always be in String format (or possibly a Buffer?)

## Features

* sync and async methods, except for `leveled(path)`, which opens the db
  syncronously
* `put`s are batched automatically since _leveldb_ only supports one put at
  at time. That makes inserting way faster (see benchmarks)
* `get`s are always concurrent

## Usage

```javascript
var leveldb = require('leveled');

var db = leveldb('/tmp/mydb');

db.putSync('foo', 'bar')

db.get('some', function (err, val) {
  val == 'value';
})

db
  .batch()
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
