#ifndef LUA_H
#define LUA_H

#include "core/reference.h"
#include "core/bind/core_bind.h"

#include <lua.hpp>
#include <string>
#include <thread>
#include <mutex>

class Lua : public Reference {
  GDCLASS(Lua, Reference);

protected:
  static void _bind_methods();

public:
  Lua();
  ~Lua();
  
  void exposeFunction(Object *instance, String function, String name );
  void callFunction( String function_name, Array args, bool protected_call = true , Object* CallbackCaller = nullptr , String callback = String() );
  bool luaFunctionExists(String function_name);
  void doFile( String fileName, bool protected_call = true , Object* CallbackCaller = nullptr , String callback = String() );
  void doString( String code, bool protected_call = true , Object* CallbackCaller = nullptr , String callback = String() );
  void setThreaded(bool thread);
  void killAll();

  static void runLua( lua_State *L , String code, bool protected_call , Object* CallbackCaller , String callback, bool *executing );

  bool pushVariant(Variant var);
  bool pushGlobalVariant(Variant var, String name);
  
  Variant popVariant();
  Variant getVariant(int index = -1);
    

  // Lua functions
  static void LineHook(lua_State *L, lua_Debug *ar);
  static int luaPrint(lua_State* state);

private:
  lua_State *state;
  bool threaded;
  bool executing;

private:
  void exposeConstructors(  );
  void createVector2Metatable(  );
  void createVector3Metatable(  );
  void createColorMetatable(  );

  static void handleError( lua_State *L , int lua_error );

};

#endif