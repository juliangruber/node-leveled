var Leveled = require('../');
var assert = require('assert');
var exec = require('child_process').exec;

var leveled = new Leveled("/tmp/foo");

// bench -----------------------------------

var count = 1200000;
var val = '1337,1337,1337,1337,1337';

function put(cb) {
  start = Date.now();

  var written = 0;
  for (var i = 0; i < count; i++) {
    leveled.put(i, val, function (err) {
      if (err) throw err;
      if (++written == count) {
        duration = Date.now()-start;
        log(count, 'put', duration, count);
        if (cb) cb();
      }
    })
  }
}

function putQueue(cb) {
  leveled.use(leveled.queue(leveled))

  start = Date.now();

  var written = 0;
  for (var i = 0; i < count; i++) {
    leveled.put(i, val, function (err) {
      if (err) throw err;
      if (++written == count) {
        duration = Date.now()-start;
        log(count, 'queue', duration, count);
        if (cb) cb();
      }
    })
  }
}

/*
 * Get it started
 */

console.log('\n  benchmarking with ' + humanize(count) + ' records, ' + val.length + ' chars each\n');

if (process.argv[2] == 'queue') return putQueue(function () { console.log() })
put(function () { console.log() })

/*
 * Utility functions
 */

function log(num, op, dur, count) {
  console.log([
    pad(op, 13),
    ':',
    pad(humanize(Math.floor(1000/dur * num)), 8),
    {'queue' : 'w', 'put' : 'w'}[op] + '/s',
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
