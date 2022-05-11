#ifndef LUA_H
#define LUA_H

#include "core/object/ref_counted.h"
#include "core/core_bind.h"
#include "luasrc/lua.hpp"
#include "luaError.h"

#include <string>

class Lua : public RefCounted {
  GDCLASS(Lua, RefCounted);

protected:
  static void _bind_methods();

public:
  Lua();
  ~Lua();
  void bindLibs(Array libs);
  void exposeFunction(Callable func, String name);
  void exposeObjectConstructor(Object* obj, String name);

  bool luaFunctionExists(String function_name);
  
  Variant getVariant(int index = -1) const;
  Variant pullVariant(String name);
  Variant callFunction(String function_name, Array args);

  LuaError* doFile(String fileName);
  LuaError* doString(String code);
  LuaError* pushVariant(Variant var) const;
  LuaError* pushGlobalVariant(Variant var, String name);

  // Lua functions
  static int luaErrorHandler(lua_State* state);
  static int luaPrint(lua_State* state);
  static int luaExposedFuncCall(lua_State* state);
  static int luaUserdataFuncCall(lua_State* state);
  static int luaCallableCall(lua_State* state);

  LuaError* handleError(int lua_error) const;

private:
  lua_State *state;
  
private:
  LuaError* execute(int handlerIndex);

  void exposeConstructors();
  void createVector2Metatable();
  void createVector3Metatable();
  void createColorMetatable();
  void createRect2Metatable();
  void createPlaneMetatable();
  void createObjectMetatable();
  void createCallableMetatable();
};

#endif