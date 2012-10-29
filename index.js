var binding = require('./build/Release/leveled');

module.exports = Leveled;

function Leveled(path) {
  if (!(this instanceof Leveled)) return new Leveled(path)
  var db = this.db = new binding.Db(path)
  this.queue = new Queue(this)
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
 * TODO: Do this in c, crossing the c/js boundary is waaay too expensive
 */
Leveled.prototype.put = function (key, val, cb) {
  if (typeof key == 'undefined' || typeof val == 'undefined') {
    return cb(new Error('key and value required'))
  }
  this.queue.push({
    key : key,
    val : val,
    cb : cb
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

function Queue(leveled) {
  this.leveled = leveled
  this.queue = []
  this.processing = false
}

Queue.prototype.push = function (obj) {
  this.queue.push(obj)
  if (!this.processing) this.process()
}

Queue.prototype.process = function () {
  var self = this
  self.processing = true
  
  var queue = self.queue.slice()
  self.queue = []

  var batch = self.leveled.createBatch()
  var len = queue.length

  for (var i = 0; i < len; i++) {
    batch.put(queue[i].key, queue[i].val)
  }

  batch.write(function (err) {
    for (var i = 0; i < len; i++) {
      if (queue[i].cb) queue[i].cb(err)
    }
    self.processing = false
    if (self.queue.length) self.process()
  })
}
