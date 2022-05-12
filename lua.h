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
  
  bool luaFunctionExists(String function_name);
  
  Variant getVariant(int index = -1) const;
  static Variant getVariant(int index, lua_State* L, Ref<RefCounted> obj);
  Variant pullVariant(String name);
  Variant callFunction(String function_name, Array args);

  LuaError* doFile(String fileName);
  LuaError* doString(String code);
  LuaError* pushVariant(Variant var) const;
  static LuaError* pushVariant(Variant var, lua_State* state);
  LuaError* pushGlobalVariant(Variant var, String name);
  LuaError* exposeObjectConstructor(Object* obj, String name);
  

  lua_State* newThread();
  lua_State* getState();

  LuaError* handleError(int lua_error) const;
  static LuaError* handleError(int lua_error, lua_State* state);
  static LuaError* handleError(const StringName &func, Callable::CallError error, const Variant** p_arguments, int argc);

  // Lua functions
  static int luaErrorHandler(lua_State* state);
  static int luaPrint(lua_State* state);
  static int luaExposedFuncCall(lua_State* state);
  static int luaUserdataFuncCall(lua_State* state);
  static int luaCallableCall(lua_State* state);

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