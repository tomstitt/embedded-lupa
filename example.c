#include <stdio.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <Python.h>

#include "_lupa.h"

// creates a repl with readline
const char * py_bootstrap = 
  "lua_globals = lua.globals()\n"
  "import code\n"
  "import readline\n"
  "import rlcompleter\n"
  "completer = rlcompleter.Completer(globals())\n"
  "readline.set_completer(completer.complete)\n"
  "readline.parse_and_bind('tab: complete')\n"
  "code.InteractiveConsole(globals()).interact()";

const char * modname = "_lupa";

int main() {
  // create lua state and load standard lubs
  lua_State * L = luaL_newstate();
  luaL_openlibs(L);

  // create a Lua global string
  lua_pushstring(L, "ripe");
  lua_setglobal(L, "my_str");

  // python 3 specific module loading
#if PY_MAJOR_VERSION >= 3
  if (PyImport_AppendInittab(modname, PyInit__lupa)) {
    fprintf(stderr, "python error: failed add %s to init table\n", modname);
    return 1;
  }
#endif

  Py_Initialize();
  
  if (Py_IsInitialized() == 0) {
    fprintf(stderr, "python error: python isn't initialized\n");
    return 1;
  }

#if PY_MAJOR_VERSION < 3
  init_lupa();
#else
  // doesn't work to call PyInit_modname directly:
  // https://cython.readthedocs.io/en/latest/src/userguide/external_C_code.html
  // > In Python 3.x, calling the module init function directly should be avoided. 
  // > Instead, use the inittab mechanism...
  if (!PyImport_ImportModule(modname)) {
    fprintf(stderr, "python error: failed to import %s\n", modname);
    return 1;
  }
#endif

  // create a Lupa::LuaRuntime from our lua_State
  PyObject * py_lua_obj = (PyObject*)luaruntime_from_state(L);
  if (py_lua_obj == NULL) {
    fprintf(stderr, "python error: unable to create new LuaRuntime\n");
    return 1;
  }

  // get the __main__ object
  PyObject * py_main_obj = PyImport_AddModule("__main__");
  if (py_main_obj == NULL) {
    fprintf(stderr, "python error: PyImport_AddModule on __main__ is NULL\n");
    return 1;
  }

  // set the python global 'lua' to our LuaRuntime
  if (PyObject_SetAttrString(py_main_obj, "lua", py_lua_obj) != 0) {
    fprintf(stderr, "python error: setattr on __main__ failed\n");
    return 1;
  }

  // load the repl
  PyRun_SimpleString(py_bootstrap);
  
  Py_Finalize();
  lua_close(L);
};
