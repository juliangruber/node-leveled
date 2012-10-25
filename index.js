var binding = require('./build/Release/leveled');

var Leveled = function(path) {
  this.db = new binding.Db(path)
}

Leveled.prototype.get = function (key, cb) {
  return this.db.get(key, cb)
}

Leveled.prototype.set = function (key, val, cb) {
  return this.db.set(key, val, cb)
}

// -----------------------------------------

var leveled = new Leveled("/tmp/foo");

var start = Date.now();
var toWrite = 120000;
var batches = 1;

for (var i = 0, len = toWrite * batches; i < len; i++) {
  leveled.set(i+'', '1337,1337,1337,1337,1337');
}
var duration = Date.now()-start;
console.log([
  toWrite * batches, ' records written in ', duration, 'ms (',
  Math.floor(1000/duration * toWrite * batches) +' w/s)'
].join(''));

// TODO: humanize
