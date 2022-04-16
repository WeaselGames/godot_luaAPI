#include "register_types.h"
#include "lua.h"
#include "luaDrawNode.h"

void register_lua_types(){
	ClassDB::register_class<Lua>();
	ClassDB::register_class<LuaDrawNode>();
}

void unregister_lua_types() {
}