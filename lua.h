#ifndef LUA_H
#define LUA_H

#include "core/reference.h"
#include "core/bind/core_bind.h"

#include <lua.hpp>
#include <map>
#include <string>

class Lua : public Reference {
  GDCLASS(Lua, Reference);

protected:
  static void _bind_methods();

public:
  Lua();
  ~Lua();
  
  void exposeFunction(Object *instance, String function, String name);
  void doFile(Object *instance, String fileName, String callback = String());
  void doString(Object *instance, String code, String callback = String());
  void setThreaded(bool thread);

  static void runLua(Object *instance, String code, String callback, lua_State *L);

  bool pushVariant(Variant var);
  bool pushGlobalVariant(Variant var, String name);
  
  Variant popVariant();
  Variant getVariant(int index = -1);
    

  // Lua functions
  static int luaPrint(lua_State* state);

private:
  lua_State *state;
  bool threaded;
};

#endif