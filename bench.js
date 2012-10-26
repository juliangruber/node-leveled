var Leveled = require('./');
var leveled = new Leveled("/tmp/foo");

// bench -----------------------------------

var start = Date.now();
var toWrite = 120000;
var batches = 1;

for (var i = 0, len = toWrite * batches; i < len; i++) {
  leveled.putSync(i+'', '1337,1337,1337,1337,1337');
}
var duration = Date.now()-start;
console.log([
  toWrite * batches, ' records written in ', duration, 'ms (',
  Math.floor(1000/duration * toWrite * batches) +' w/s)'
].join(''));
