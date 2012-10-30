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
  describe('.putSync(key, value)', function() {
    it('should require key and value', function() {
      leveled.putSync.bind(leveled).should.throw()
      leveled.putSync.bind(leveled, 'key').should.throw()
    })
    it('should store a value', function() {
      leveled.putSync("key", "value")
      leveled.getSync("key").should.equal("value")
    })
    it('should support integer keys', function() {
      leveled.putSync(1, "one")
      leveled.getSync(1).should.equal("one")
    })
    it('should store only strings', function() {
      leveled.putSync.bind(leveled, 1, 1).should.throw()
      leveled.putSync.bind(leveled, 1, {}).should.throw()
    })
    it('should require key argument', function() {
      leveled.putSync.bind(leveled).should.throw()
    })
  })
  describe('.put(key, value[, cb])', function() {
    it('should store a value', function(done) {
      leveled.put('key', 'value', function (err) {
        should.not.exist(err)
        leveled.getSync('key').should.equal('value')
        done()
      })
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
      leveled.putSync('key', 'value')
      leveled.get('key', function (err, value) {
        if (err) throw err
        value.should.equal('value')
        done()
      })
    })
  })
  describe('.getSync(key)', function() {
    it('should require key argument', function() {
      leveled.getSync.bind(leveled).should.throw()
    })
  })
  describe('.batch()', function() {
    it('should create one', function() {
      should.exist(leveled.batch())
    })
  })
  describe('.delSync(key)', function() {
    it('should delete', function() {
      leveled.putSync('foo', 'bar')
      leveled.getSync('foo').should.equal('bar')
      leveled.getSync('foo').should.equal('bar')
      leveled.delSync('foo')
      leveled.getSync.bind(leveled, 'foo').should.throw()
    })
  })
  describe('.del(key)', function() {
    it('should delete', function(done) {
      leveled.putSync('foo', 'bar')
      leveled.getSync('foo').should.equal('bar')
      leveled.del('foo', function (err) {
        should.not.exist(err)
        leveled.getSync.bind(leveled, 'foo').should.throw()
        done()
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
    it('should work', function() {
      batch.put('foo', 'bar123')
      batch.writeSync()
      leveled.getSync('foo').should.equal('bar123')
    })
    it('should take only string values', function() {
      batch.put.bind(batch, 'key', 1).should.throw()
    })
  })
  describe('.del', function() {
    it('should work', function() {
      batch.del('foo')
      batch.writeSync()
      leveled.getSync.bind(leveled, 'foo').should.throw()
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
