#include "register_types.h"
#include "lua.h"
#include "luaError.h"
#include "luaThread.h"
#include "luaCallable.h"

void initialize_lua_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		ClassDB::register_class<Lua>();
		ClassDB::register_class<LuaThread>();
		ClassDB::register_class<LuaError>();
	}
}

void uninitialize_lua_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}