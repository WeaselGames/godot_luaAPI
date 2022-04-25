#ifndef LUA_H
#define LUA_H

#include "core/object/ref_counted.h"
#include "core/core_bind.h"

#include "luasrc/lua.hpp"
#include <string>

class Lua : public RefCounted {
  GDCLASS(Lua, RefCounted);

protected:
  static void _bind_methods();

public:
  Lua();
  ~Lua();
  void doFile(String fileName);
  void doString(String code);
  void bindLibs(Array libs);
  bool pushVariant(Variant var);
  void exposeFunction(Callable func, String name); 
  void setErrorHandler(Callable errorHandler);
  void exposeObjectConstructor(Object* obj, String name);

  bool pushGlobalVariant(Variant var, String name);
  bool luaFunctionExists(String function_name);
  
  Variant getVariant(int index = -1);
  Variant pullVariant(String name);
  Variant callFunction(String function_name, Array args );

  Callable getCallable(int index);

  // Lua functions
  static int luaPrint(lua_State* state);
  static int luaExposedFuncCall(lua_State* state);
  static int luaUserdataFuncCall(lua_State* state);

private:
  lua_State *state;
  Vector<Callable> callables;
  Callable errorHandler;

private:
  void execute();
  void exposeConstructors();
  void createVector2Metatable();
  void createVector3Metatable();
  void createColorMetatable();
  void createRect2Metatable();
  void createPlaneMetatable();
  void createObjectMetatable();

  void handleError(int lua_error);
};

#endif