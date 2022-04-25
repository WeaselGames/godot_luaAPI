#ifndef LUA_H
#define LUA_H

#include "core/object/ref_counted.h"
#include "core/core_bind.h"

#include "luasrc/lua.hpp"
#include <string>
#include <thread>
#include <mutex>
#include <map>

class Lua : public RefCounted {
  GDCLASS(Lua, RefCounted);

protected:
  static void _bind_methods();

public:
  Lua();
  ~Lua();
  
  void bindLibs(Array libs);
  void setErrorHandler(Callable errorHandler);
  void exposeFunction(Callable func, String name); 
  void doFile(String fileName);
  void doString(String code);
  void exposeObjectConstructor(Object* obj, String name);


  bool pushVariant(Variant var);
  bool pushGlobalVariant(Variant var, String name);
  bool luaFunctionExists(String function_name);
  
  Variant pullVariant(String name);
  Variant getVariant(int index = -1);
  Variant callFunction( String function_name, Array args );

  Callable getCallable(int index);
  lua_State* getState();

  // Lua functions
  static void LineHook(lua_State *L, lua_Debug *ar);
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