//insert the Headername here
#ifndef CALCULATORREF_H
#define CALCULATORREF_H

#include "core/reference.h"

#include <lua.hpp>

class Lua : public Reference {
  GDCLASS(Lua, Reference);

protected:
  static void _bind_methods();

public:
  Lua();
  void run(String code);
};

#endif