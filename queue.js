module.exports = Queue;

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

  var batch = self.leveled.batch()
  var len = queue.length

  for (var i = 0; i < len; i++) {
    batch[queue[i].op](queue[i].key, queue[i].val)
  }

  batch.write(function (err) {
    for (var i = 0; i < len; i++) {
      if (queue[i].cb) queue[i].cb(err)
    }
    self.processing = false
    if (self.queue.length) self.process()
  })
}
