#include "register_types.h"
#include "lua.h"
#include "luaDrawNode.h"

void register_lua_types(){
	ClassDB::register_class<Lua>();
}

void unregister_lua_types() {
}