var binding = require('./build/Release/leveled');

var Leveled = function(path) {
  this.db = new binding.Db()
}

Leveled.prototype.get = function (key, cb) {
  this.db.get(key, cb)
}

Leveled.prototype.set = function (key, val, cb) {
  this.db.set(key, val, cb)
}

var leveled = new Leveled();
console.log(leveled.db.get);
