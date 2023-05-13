# Embedded Lupa

Embedding Lupa in a Python-embedded C/C++ application with an existing Lua State.

See the example (*example.cpp*). which requires Lua and Python.

```shell
> ./example
...
>>> lua_globals.my_str
'ripe'
```

To build:

```shell
# lua.root is only needed when building the example
make python=/path/to/your/python [lua.include=/path/to/your/lua/install/include] [lua.lib=/path/to/your/lua/install/lib] [CC=]
```

Tested with Python 2.7 and 3.6.
