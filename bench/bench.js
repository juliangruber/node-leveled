var Leveled = require('../');
var assert = require('assert');

var leveled = new Leveled("/tmp/foo");

// bench -----------------------------------

var count = 120000;
var val = '1337,1337,1337,1337,1337';

function putSync () {
  var start = Date.now();

  for (var i = 0; i < count; i++) {
    leveled.putSync(i, val);
  }

  var duration = Date.now()-start;
  log(true, count, 'put', duration, count);
}

function readSync () {
  start = Date.now();

  for (var i = 0; i < count; i++) {
    assert(leveled.getSync(i) == val);
  }

  duration = Date.now()-start;
  log(true, count, 'get', duration, count);
}

function putAsync(cb) {
  start = Date.now();

  var written = 0;
  for (var i = 0; i < count; i++) {
    leveled.put(i, val, function (err) {
      if (err) throw err;
      if (++written == count) {
        duration = Date.now()-start;
        log(false, count, 'put', duration, count);
        if (cb) cb();
      }
    })
  }
}

function readAsync (cb) {
  start = Date.now();

  var received = 0;
  for (var i = 0; i < count; i++) {
    leveled.get(i, function (err, value) {
      if (err) throw err;
      assert(value == val);
      if (++received == count) {
        duration = Date.now()-start;
        log(false, count, 'get', duration, count);
        if (cb) cb()
      }
    })
  }
}

function batchSync () {
  var start = Date.now();

  var batch = leveled.batch();

  for (var i = 0; i < count; i++) {
    batch.put(i, val);
  }

  batch.writeSync();

  var duration = Date.now()-start;
  log(true, count, 'batch', duration, count);
}

function batchAsync(cb) {
  start = Date.now();

  var batch = leveled.batch();
  for (var i = 0; i < count; i++) {
    batch.put(i, val);
  }

  batch.write(function (err) {
    if (err) throw err;
    duration = Date.now()-start;
    log(false, count, 'batch', duration, count);
    if (cb) cb();
  });
}

/*
 * Get it started
 */

console.log('\n  benchmarking with ' + humanize(count) + ' records, ' + val.length + ' chars each\n');

putAsync(function () {
  putSync()
  batchAsync(function () {
    batchSync()
    readAsync(function () {
      readSync()
      console.log()
    })
  })
})

/*
 * Utility functions
 */

var last;
function log(sync, num, op, dur, count) {
  if (last && last != op) console.log();
  last = op;
  console.log([
    pad(op + (sync? 'Sync' : ''), 13),
    ':',
    pad(humanize(Math.floor(1000/dur * num)), 8),
    {'batch' : 'w', 'put' : 'w', 'get' : 'r'}[op] + '/s',
    'in ' + pad(humanize(dur), 6) + 'ms'
   ].join(' '));
}

function pad(str, len) {
  str = str.toString();
  return Array(len - str.length + 1).join(' ') + str;
}

function humanize(n, options){
  options = options || {};
  var d = options.delimiter || ',';
  var s = options.separator || '.';
  n = n.toString().split('.');
  n[0] = n[0].replace(/(\d)(?=(\d\d\d)+(?!\d))/g, '$1' + d);
  return n.join(s);
}
