#include "register_types.h"
#include "src/classes/luaAPI.h"
#include "src/classes/luaCallableExtra.h"
#include "src/classes/luaCoroutine.h"
#include "src/classes/luaError.h"
#include "src/classes/luaObjectMetatable.h"
#include "src/classes/luaTuple.h"

#ifdef LAPI_GDEXTENSION
using namespace godot;
#endif

void initialize_luaAPI_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	ClassDB::register_class<LuaAPI>();
	ClassDB::register_class<LuaCallableExtra>();
	ClassDB::register_class<LuaCoroutine>();
	ClassDB::register_class<LuaError>();
	ClassDB::register_class<LuaObjectMetatable>();
	ClassDB::register_class<LuaDefaultObjectMetatable>();
	ClassDB::register_class<LuaTuple>();
}

void uninitialize_luaAPI_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

#ifdef LAPI_GDEXTENSION
extern "C" {
// Initialization.

GDExtensionBool GDE_EXPORT luaAPI_library_init(GDExtensionInterfaceGetProcAddress p_interface, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	GDExtensionBinding::InitObject init_obj(p_interface, p_library, r_initialization);

	init_obj.register_initializer(initialize_luaAPI_module);
	init_obj.register_terminator(uninitialize_luaAPI_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
#endif
