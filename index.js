var binding = require('./build/Release/leveled');

module.exports = Leveled;

function Leveled(path) {
  if (!(this instanceof Leveled)) return new Leveled(path)

  var db = new binding.Db(path)

  db.batch = function () {
    var batch = new binding.Batch()
    batch.writeSync = function () {
      return db.writeSync(batch)
    }
    batch.write = function (cb) {
      return db.write(batch, cb)
    }
    return batch
  }

  return db
}
