module.exports = function (leveled) {
  if (!leveled) throw new Error('leveled required')
  var queue = new Queue(leveled)

  return function (req, res, next) {
    if (req.method != 'put') return next();

    var end = res.end
    res.end = function (err, data) {
      queue.push({
        key : req.key,
        val : req.val,
        cb : end
      })
    }

    next()
  }
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
  console.log('processing');
  var self = this
  self.processing = true
  
  var queue = self.queue.slice()
  self.queue = []
  console.log('queue sliced')
  var batch = self.leveled.batch()
  console.log('batch created')
  var len = queue.length

  console.log('len', len)
  for (var i = 0; i < len; i++) {
    batch.put(queue[i].key, queue[i].val)
  }
  console.log('batch filled')

  batch.write(function (err) {
    console.log('batch written')
    for (var i = 0; i < len; i++) {
      if (queue[i].cb) queue[i].cb(err)
    }
    self.processing = false
    if (self.queue.length) self.process()
  })
}
