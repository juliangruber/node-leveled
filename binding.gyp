{
  "targets" : [
    {
      "target_name" : "leveled",
      "sources" : ["src/leveled.cc"],
      "dependencies" : [
        "deps/leveldb/binding.gyp:leveldb"
      ]
    }
  ]
}
