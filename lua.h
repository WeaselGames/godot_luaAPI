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
  void exposeFunction(Callable func, String name); 
  void setErrorHandler(Callable errorHandler);
  void exposeObjectConstructor(Object* obj, String name);

  bool pushVariant(Variant var) const;
  bool pushGlobalVariant(Variant var, String name);
  bool luaFunctionExists(String function_name);
  
  Variant getVariant(int index = -1) const;
  Variant pullVariant(String name);
  Variant callFunction(String function_name, Array args);

  // Lua functions
  static int luaPrint(lua_State* state);
  static int luaExposedFuncCall(lua_State* state);
  static int luaUserdataFuncCall(lua_State* state);
  static int luaCallableCall(lua_State* state);

  Callable errorHandler;
  void handleError(int lua_error) const;

private:
  lua_State *state;
  
private:
  void execute();
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