var binding = require('./build/Release/leveled');

module.exports = Leveled;

function Leveled(path) {
  if (!(this instanceof Leveled)) return new Leveled(path)

  var self = this;
  self.db = new binding.Db(path)
  self.middlewares = []

  Object.keys(self.db).forEach(function (op) {
    console.log('op', op)
    self[op] = self.db[op].bind(self.db)
  })
}

Leveled.prototype.batch = function () {
  var db = this.db
  var batch = new binding.Batch()
  batch.writeSync = function () {
    return db.writeSync(batch)
  }
  batch.write = function (cb) {
    return db.write(batch, cb)
  }
  return batch
}

Leveled.prototype.use = function (middleware) {
  this.middlewares.push(middleware)
}
