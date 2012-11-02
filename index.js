var binding = require('./build/Release/leveled');
var fs = require('fs');

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
      end : cb
    }

    function next() {
      var layer = stack[index++]

      if (!layer) {
        switch (method) {
          case 'get': db.get(req.key, res.end); break;
          case 'del': db.del(req.key, res.end); break;
          case 'put': db.put(req.key, req.val, res.end); break;
        }
        return
      }

      layer(req, res, next)
    }

    next()
  }
}

Leveled.prototype.batch = function () {
  var db = this.db
  var batch = new binding.Batch()
  batch.write = function (cb) {
    return db.write(batch, cb)
  }
  return batch
}

Leveled.prototype.use = function (middleware) {
  this.middlewares.push(middleware)
}

Leveled.prototype.clear = function () {
  this.middlewares = []
}

/*
 * Export middlewares
 */

fs.readdirSync(__dirname + '/lib/middleware').forEach(function (file) {
  if (!/\.js$/.test(file)) return
  Leveled.prototype[file.split('.js')[0]] = require('./lib/middleware/' + file);
})
