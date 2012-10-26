var binding = require('./build/Release/leveled');

module.exports = Leveled;

function Leveled(path) {
  if (!(this instanceof Leveled)) return new Leveled(path)
  this.db = new binding.Db(path)
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
