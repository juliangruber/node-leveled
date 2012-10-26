var should = require('should');
var Leveled = require('../');

var leveled = new Leveled('/tmp/foo');

describe('leveled', function() {
  describe('new Leveled(path)', function() {
    it('should return a db', function() {
      (new Leveled('/tmp/foo1')).should.be.ok;
      Leveled('/tmp/foo2').should.be.ok;
    })
    it('should require a valid path', function() {
      Leveled.bind(this, '/a/b/c/d/e').should.throw();
    })
  })
  describe('.putSync(key, value)', function() {
    it('should require key and value', function() {
      leveled.putSync.bind(leveled).should.throw();
      leveled.putSync.bind(leveled, 'key').should.throw();
    })
    it('should store a value', function() {
      leveled.putSync("key", "value");
      leveled.getSync("key").should.equal("value");
    })
    it('should support integer keys', function() {
      leveled.putSync(1, "one");
      leveled.getSync(1).should.equal("one");
    })
    it('should store integer values')
    it('should store js objects')
    it('should require key argument', function() {
      leveled.putSync.bind(leveled).should.throw();
    })
  });
  describe('.put(key, value[, cb])', function() {
    it('should store a value', function(done) {
      leveled.put('key', 'value', function (err) {
        should.not.exist(err);
        leveled.getSync('key').should.equal('value');
        done();
      })
    })
    it('should require key and value', function() {
      leveled.put.bind(leveled).should.throw();
      leveled.put.bind(leveled, 'key').should.throw();
      leveled.put.bind(leveled, 'key', 'value').should.not.throw();
    })
  })
  describe('.get(key, val)', function() {
    it('should require both arguments', function() {
      leveled.get.bind(leveled).should.throw();
      leveled.get.bind(leveled, 'key').should.throw();
    })
    it('should get a value', function(done) {
      leveled.putSync('key', 'value');
      leveled.get('key', function (err, value) {
        console.log(arguments);
        //value.should.equal('value');
        done();
      })
    })
  })
  describe('.getSync(key)', function() {
    it('should require key argument', function() {
      leveled.getSync.bind(leveled).should.throw();
    })
  })
})
