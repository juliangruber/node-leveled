{
  "targets" : [
    {
      "target_name" : "leveled",
      "sources" : ["src/leveled.cc", "src/batch.cc"],
      "dependencies" : [
        "deps/leveldb/binding.gyp:leveldb"
      ]
    }
  ]
}
