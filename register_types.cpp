#include "register_types.h"
#include "src/classes/luaAPI.h"
#include "src/classes/luaError.h"
#include "src/classes/luaThread.h"
#include "src/classes/luaCallable.h"
#include "src/classes/luaTuple.h"

void initialize_luaAPI_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
		return;
	
	ClassDB::register_class<LuaAPI>();
	ClassDB::register_class<LuaThread>();
	ClassDB::register_class<LuaError>();
	ClassDB::register_class<LuaTuple>();
}

void uninitialize_luaAPI_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
		return;
}