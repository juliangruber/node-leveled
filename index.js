var binding = require('./build/Release/leveled');

module.exports = Leveled;

function Leveled(path) {
  if (!(this instanceof Leveled)) return new Leveled(path)
  var db = this.db = new binding.Db(path)
  this.opts = {}
}

Leveled.prototype.get = function (key, cb) {
  return this.db.get.apply(this.db, arguments)
}

Leveled.prototype.getSync = function (key, cb) {
  return this.db.getSync.apply(this.db, arguments)
}

Leveled.prototype.put = function (key, val, cb) {
  return this.db.put.apply(this.db, arguments)
}

Leveled.prototype.putSync = function (key, val, cb) {
  return this.db.putSync.apply(this.db, arguments)
}

Leveled.prototype.del = function (key, cb) {
  return this.db.del.apply(this.db, arguments)
}

Leveled.prototype.delSync = function (key) {
  return this.db.delSync.apply(this.db, arguments)
}

Leveled.prototype.batch = function () {
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
