var should = require('should')
var Leveled = require('../')
var binding = require('../build/Release/leveled')

var leveled = new Leveled('/tmp/foo')

describe('leveled', function() {
  describe('new Leveled(path)', function() {
    it('should return a db', function() {
      (new Leveled('/tmp/foo1')).should.be.ok
      Leveled('/tmp/foo2').should.be.ok
    })
    it('should require a valid path', function() {
      Leveled.bind(this, '/a/b/c/d/e').should.throw()
    })
  })
  describe('.put(key, value[, cb])', function() {
    it('should store a value', function(done) {
      leveled.put('key', 'value', function (err) {
        should.not.exist(err)
        leveled.get('key', function (err, val) {
          if (err) throw err
          val.should.equal('value')
          done()
        })
      })
    })
    it('should support integer keys', function(done) {
      leveled.put(1, "one", function (err) {
        if (err) throw err
        leveled.get(1, function (err, val) {
          val.should.equal('one')
          done()
        })
      })
    })
    it('should store only strings', function() {
      leveled.put.bind(leveled, 1, 1).should.throw()
      leveled.put.bind(leveled, 1, {}).should.throw()
    })
    it('should require key and value', function() {
      leveled.put.bind(leveled).should.throw()
      leveled.put.bind(leveled, 'key').should.throw()
      leveled.put.bind(leveled, 'key', 'value').should.not.throw()
    })
  })
  describe('.get(key, val)', function() {
    it('should require both arguments', function() {
      leveled.get.bind(leveled).should.throw()
      leveled.get.bind(leveled, 'key').should.throw()
    })
    it('should get a value', function(done) {
      leveled.put('key', 'value', function (err) {
        if (err) throw err;
        leveled.get('key', function (err, value) {
          if (err) throw err
          value.should.equal('value')
          done()
        })
      })
    })
  })
  describe('.batch()', function() {
    it('should create one', function() {
      should.exist(leveled.batch())
    })
  })
  describe('.del(key)', function() {
    it('should delete', function(done) {
      leveled.put('foo', 'bar', function (err) {
        if (err) throw err;
        leveled.get('foo', function (err, val) {
          if (err) throw err;
          val.should.equal('bar'); 
          leveled.del('foo', function (err) {
            should.not.exist(err)
            leveled.get('foo', function (err, val) {
              err.should.be.ok
              done()
            })
          }) 
        })
      })
    })
  })
})

var batch
describe('Batch', function() {
  beforeEach(function() {
    batch = leveled.batch()
  })
  describe('.put', function() {
    it('should work', function(done) {
      batch.put('foo', 'bar123')
      batch.write(function (err) {
        if (err) throw err
        leveled.get('foo', function (err, val) {
          if (err) throw err
          val.should.equal('bar123')
          done()
        })
      })
    })
    it('should take only string values', function() {
      batch.put.bind(batch, 'key', 1).should.throw()
    })
  })
  describe('.del', function() {
    it('should work', function(done) {
      batch.del('foo')
      batch.write(function (err) {
        if (err) throw err
        leveled.get('foo', function (err, val) {
          should.exist(err)
          done()
        })
      })
    })
  })
  describe('.write', function() {
    it('should work multiple time', function(done) {
      var batch1 = leveled.batch()
      var batch2 = leveled.batch()
      batch1.put('foo', 'bar')
      batch2.put('bar', 'baz')
      batch1.write(function (err) {
        if (err) throw err
        batch2.write(function (err) {
          if (err) throw err
          done()
        })
      })
    })
  })
})

describe('middleware', function() {
  it('should export them', function() {
    should.exist(leveled.queue)
  })
  it('should add', function(done) {
    leveled.use(function (req, res, next) {
      next()
    })
    leveled.put('foo', 'bar', function (err) {
      if (err) throw err
      leveled.get('foo', function (err, val) {
        if (err) throw err
        val.should.equal('bar')
        done()
      })
    })
  })

  it('should transform req', function(done) {
    leveled.use(function (req, res, next) {
      if (req.method != 'put') return next()
      req.val = req.val.toUpperCase();
      next()
    })
    leveled.put('foo', 'bar', function (err) {
      if (err) throw err
      leveled.get('foo', function (err, val) {
        if (err) throw err
        val.should.equal('BAR')
        done()
      })
    })
  })

  it('should transform res', function(done) {
    leveled.use(function (req, res, next) {
      var end = res.end

      if (req.method == 'put') req.val = compress(req.val)
      if (req.method == 'get') res.end = function (err, data) {
        end(null, decompress(data))
      }
      next()
    })

    function compress(data) {
      return 'compressed' + data;
    }

    function decompress(data) {
      return data.split('compressed')[1];
    }

    leveled.put('fab', 'fob', function (err) {
      if (err) throw err
      leveled.get('fab', function (err, val) {
        if (err) throw err
        val.should.equal('FOB')
        done()
      })
    })
  })
})
