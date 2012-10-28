var binding = require('./build/Release/leveled');
var FunctionQueue = require('function-queue');

module.exports = Leveled;

function Leveled(path) {
  if (!(this instanceof Leveled)) return new Leveled(path)
  this.db = new binding.Db(path)
  this.queue = FunctionQueue();
}

Leveled.prototype.get = function (key, cb) {
  return this.db.get.apply(this.db, arguments)
}

Leveled.prototype.getSync = function (key, cb) {
  return this.db.getSync.apply(this.db, arguments)
}

/**
 * LevelDB only allows for one put operation at a time
 * so we have to queue this
 *
 * TODO: Do this in c, crossing the c/js boundary is waaay too expansive
 */
Leveled.prototype.put = function (key, val, cb) {
  if (typeof key == 'undefined' || typeof val == 'undefined') {
    return cb(new Error('key and value required'))
  }
  var db = this.db
  this.queue.push(function (next) {
    db.put.call(db, key, val, function () {
      if (cb) cb()
      next()
    })
  })
}

Leveled.prototype.putSync = function (key, val, cb) {
  return this.db.putSync.apply(this.db, arguments)
}

Leveled.prototype.createBatch = function () {
  var db = this.db;
  var batch = new binding.Batch()
  batch.writeSync = function () {
    return db.writeSync(batch)
  }
  batch.write = function (cb) {
    return db.write(batch, cb)
  }
  return batch
}
