#include "register_types.h"

#include "src/classes/luaAPI.h"
#include "src/classes/luaError.h"
#include "src/classes/luaThread.h"
#include "src/classes/luaTuple.h"
#include "src/classes/luaCallableExtra.h"

#ifndef LAPI_GODOT_EXTENSION

void initialize_luaAPI_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
		return;
	
	ClassDB::register_class<LuaAPI>();
	ClassDB::register_class<LuaThread>();
	ClassDB::register_class<LuaError>();
	ClassDB::register_class<LuaTuple>();
	ClassDB::register_class<LuaCallableExtra>();
}

void uninitialize_luaAPI_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
		return;
}

#else
void initialize_luaAPI_types(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
		return;
	
	ClassDB::register_class<LuaAPI>();
	ClassDB::register_class<LuaThread>();
	ClassDB::register_class<LuaError>();
	ClassDB::register_class<LuaTuple>();
	ClassDB::register_class<LuaCallableExtra>();
}

void uninitialize_luaAPI_types(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

#endif