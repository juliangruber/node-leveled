/**
 * Queue middleware
 *
 * Batches up sequential put operations.
 *
 * This can lead to a speedup of about 400%,
 * sacrificing individual response time for put
 * operations.
 *
 * $ node bench/queue.js
 *
 *     put :  133,630 w/s in    898ms
 *
 * $ node bench/queue.js queue
 *
 *   queue :  446,096 w/s in    269ms
 *
 */

module.exports = function (leveled) {
  if (!leveled) throw new Error('leveled required')
  var queue = new Queue(leveled)

  return function queue (req, res, next) {
    if (req.method != 'put') return next();

    queue.push({
      key : req.key,
      val : req.val,
      cb : res.end
    })
  }
}

/**
 * Queue
 *
 * @param {Leveled} leveled
 */

function Queue(leveled) {
  this.leveled = leveled
  this.queue = []
  this.processing = false
}

/**
 * Add an operation to the queue
 *
 * @param {Object} obj
 * @param {String} obj.key
 * @param {String} obj.val
 * @param {Function=} obj.cb
 */

Queue.prototype.push = function (obj) {
  this.queue.push(obj)
  if (!this.processing) this.process()
}

/**
 * Process all the items in the queue in a batch
 */

Queue.prototype.process = function () {
  var self = this
  self.processing = true
  
  var queue = self.queue.slice()
  self.queue = []
  var batch = self.leveled.batch()
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
