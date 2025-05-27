{
  "targets": [
    {
      "target_name": "ina219_native",
      "sources": [ 
          "src/c-src/extension.c",
          "src/c-src/fn-bindings.c",
          "src/c-src/structs.c",
          "src/ina219/src/driver_ina219.c"
      ],
      "include_dirs": [
        "/usr/local/include/node/",
        "src/c-include/",
        "src/ina219/src/"
      ],
      "cflags!": [ 
            "-Wall",               # basic warnings
            "-Wextra",             # extra warnings
            "-Wpedantic",          # strict ISO compliance
            "-Wconversion",        # implicit conversions
            "-Wshadow",            # variable shadowing
            "-Wcast-align",        # alignment casts
            "-Wformat-security",   # printf/scanf security
            "-Wnull-dereference",  # null pointer checks
            "-Werror"              # turn *all* warnings into errors
        ],
    }
  ]
}
