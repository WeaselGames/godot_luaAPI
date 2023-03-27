#ifndef LAPI_LUA_HPP
#define LAPI_LUA_HPP

#ifndef LAPI_LUAJIT

extern "C" {
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

#else

#include <luaJIT/src/lua.hpp>

#define LUA_OK 0

inline void luaL_setmetatable(lua_State *L, const char *tname) {
    luaL_getmetatable(L, tname);
    lua_setmetatable(L, -2);
}

#endif

#endif