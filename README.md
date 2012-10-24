
# node-leveled

LevelDB

## Usage

```javascript
var leveldb = require('leveled');

var db = leveldb('/tmp/mydb');

db.set('foo', 'bar')

db.get('some', function (err, val) {
  val == 'value';
})

var batch = new db.batch();
batch.set('foo', 'bar')
batch.get('foo')
batch.start(function(err, val) {
  val == 'foo';
});
```