var binding = require('./build/Release/leveled');

module.exports = Leveled;

function Leveled(path) {
  if (!(this instanceof Leveled)) return new Leveled(path)

  var self = this
  self.db = new binding.Db(path)
  self.middlewares = []

  Object.keys(binding.Db.prototype).forEach(function (op) {
    self[op] = self.handle(op)
  })
}

Leveled.prototype.handle = function (method) {
  var db = this.db
  var stack = this.middlewares

  return function (key, val, cb) {
    var index = 0

    if (typeof val == 'function') {
      cb = val
      val = undefined
    }

    var req = {
      key : key,
      val : val,
      method : method
    }

    var res = {
      write : function (data) {
        if (cb) cb(data)  
      },
      end : function (data) {
        if (data) res.write(data)
      }
    }

    function next() {
      var layer = stack[index++]

      if (!layer) {
        switch (method) {
          case 'get': db.get(req.key, res.end); break;
          case 'del': db.del(req.key, res.end); break;
          case 'put': db.put(req.key, req.val, res.end); break;
        }
      }

    }

    next()
  }
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
