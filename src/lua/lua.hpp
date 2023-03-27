#ifndef LAPI_LUA_HPP
#define LAPI_LUA_HPP

#ifndef LAPI_51

extern "C" {
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

#else

#ifndef LAPI_LUAJIT
extern "C" {
#include <lua51/lua.h>
#include <lua51/lualib.h>
#include <lua51/lauxlib.h>
}
#else
#include <luaJIT/src/lua.hpp>
#endif

#define LUA_OK 0

inline void luaL_setmetatable(lua_State *L, const char *tname) {
    luaL_getmetatable(L, tname);
    lua_setmetatable(L, -2);
}

#endif

#endif