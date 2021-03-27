//insert the Headername here
#ifndef CALCULATORREF_H
#define CALCULATORREF_H

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
  
  void run(String code);
  void exposeFunction(Object *p_instance, String function, String name);

  bool load(String fileName);
  bool pushVariant(Variant var);
  bool pushGlobalVariant(Variant var, String name);
  Variant execute(String name, Array array);
  Variant popVariant();
  Variant getVariant(int index = -1);
  

  // Lua functions
  static int luaPrint(lua_State* state);

private:
  lua_State *state;
};

#endif