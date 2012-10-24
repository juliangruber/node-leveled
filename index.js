var leveled = require('./build/Release/leveled').leveled;

var db = leveled('/tmp/path');

db.get('foo', function () {
  console.log(arguments);
});
