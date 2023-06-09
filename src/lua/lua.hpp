#ifndef LAPI_LUA_HPP
#define LAPI_LUA_HPP

#ifndef LAPI_51 // LUA 5.4 ONLY

extern "C" {
#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>
}

#else

#ifndef LAPI_LUAJIT // LUA 5.1 ONLY
extern "C" {
#include <lua51/lauxlib.h>
#include <lua51/lua.h>
#include <lua51/lualib.h>
}

#else // LUAJIT ONLY

#include <luaJIT/src/lua.hpp>

#endif
#endif

#endif
