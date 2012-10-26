var Leveled = require('../');
var assert = require('assert');

var leveled = new Leveled("/tmp/foo");

// bench -----------------------------------

var count = 120000;

function putSync () {
  console.log('putting in data (sync)');
  var start = Date.now();

  for (var i = 0; i < count; i++) {
    leveled.putSync(i, '1337,1337,1337,1337,1337');
  }

  var duration = Date.now()-start;
  console.log([
    count, ' records written in ', duration, 'ms (',
    Math.floor(1000/duration * count) +' w/s)'
  ].join(''));
}

function readSync () {
  console.log('reading data (sync)');
  start = Date.now();

  for (var i = 0; i < count; i++) {
    assert(leveled.getSync(i) == '1337,1337,1337,1337,1337');
  }

  duration = Date.now()-start;
  console.log([
    count, ' records read in ', duration, 'ms (',
    Math.floor(1000/duration * count) +' r/s)'
  ].join(''));
}

function putAsync(cb) {
  console.log('putting in data (async)');
  start = Date.now();

  var written = 0;
  for (var i = 0; i < count; i++) {
    leveled.put(i, '1337,1337,1337,1337,1337', function (err) {
      if (err) throw err;
      if (++written == count) {
        duration = Date.now()-start;
        console.log([
          count, ' records written in ', duration, 'ms (',
          Math.floor(1000/duration * count) +' w/s)'
        ].join(''));
        if (cb) cb();
      }
    })
  }
}

function readAsync (cb) {
  console.log('reading data (async)');
  start = Date.now();

  var received = 0;
  for (var i = 0; i < count; i++) {
    leveled.get(i, function (err, value) {
      if (err) throw err;
      assert(value == '1337,1337,1337,1337,1337');
      if (++received == count) {
        duration = Date.now()-start;
        console.log([
          count, ' records read in ', duration, 'ms (',
          Math.floor(1000/duration * count) +' r/s)'
        ].join(''));
        if (cb) cb()
      }
    })
  }
}

putAsync(function () {
  readAsync(function () {
    putSync()
    readSync()
  })
})