
#include "lua.h"

#include <string>

Lua::Lua(){}

void Lua::_bind_methods(){
  ClassDB::bind_method(D_METHOD("run", "code"), &Lua::run);
}

int luaPrint(lua_State* state)
{

  int args = lua_gettop(state);

  for ( int n=1; n<=args; ++n) {
    print_line(lua_tostring(state, n));
  }

  return 0;
}

void Lua::run(String code) {


  lua_State *state = luaL_newstate();

  luaL_openlibs(state);

  lua_register(state, "print", luaPrint);

  int result;

  std::wstring luaCode = code.c_str();
  const std::string luaCode2( luaCode.begin(), luaCode.end() );

  result = luaL_loadstring(state, luaCode2.c_str());

  if ( result != LUA_OK ) {
    print_line("Rip1!");
    return;
  }

  result = lua_pcall(state, 0, LUA_MULTRET, 0);

  if ( result != LUA_OK ) {
    print_line("Rip!2");
    return;
  }
}

