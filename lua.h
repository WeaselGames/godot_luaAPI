#ifndef LUA_H
#define LUA_H

// These 2 macros helps us in constructing general metamethods.
// We can use "lua" as a "Lua" pointer and arg1, arg2, ..., arg5 as Variants objects
// Check examples in createVector2Metatable
#define LUA_LAMBDA_TEMPLATE(_f_) \
 [](lua_State* inner_state) -> int {                      \
     lua_pushstring(inner_state,"__Lua");                 \
     lua_rawget(inner_state,LUA_REGISTRYINDEX);           \
     Lua* lua = (Lua*) lua_touserdata(inner_state,-1);;   \
     lua_pop(inner_state,1);                              \
     Variant arg1 = lua->getVariant(1);                             \
     Variant arg2 = lua->getVariant(2);                             \
     Variant arg3 = lua->getVariant(3);                             \
     Variant arg4 = lua->getVariant(4);                             \
     Variant arg5 = lua->getVariant(5);                             \
     _f_                                                            \
}
 
#define LUA_METAMETHOD_TEMPLATE( lua_state , metatable_index , metamethod_name , _f_ )\
lua_pushstring(lua_state,metamethod_name); \
lua_pushcfunction(lua_state,LUA_LAMBDA_TEMPLATE( _f_ )); \
lua_settable(lua_state,metatable_index-2);

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
  void addFile(String fileName);
  void addString( String code );
  void execute();
  void registerObject(Object* obj);


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

private:
  lua_State *state;
  Vector<Callable> callables;
  Callable errorHandler;
  Map<String, bool> registedObjects;

private:
  void exposeConstructors(  );
  void createVector2Metatable(  );
  void createVector3Metatable(  );
  void createColorMetatable(  );
  void createRect2Metatable(  );
  void createPlaneMetatable(  );

  void handleError( int lua_error );

};

#endif