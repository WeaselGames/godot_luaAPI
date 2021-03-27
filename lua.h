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

  bool pushVariant(Variant var);
  bool pushGlobalVariant(Variant var, String name);

  Variant doFile(String fileName);
  Variant doString(String code);
  Variant popVariant();
  Variant getVariant(int index = -1);
    

  // Lua functions
  static int luaPrint(lua_State* state);

private:
  lua_State *state;
};

#endif