#ifndef LUA_REGISTER_TYPES_H
#define LUA_REGISTER_TYPES_H
#ifndef LAPI_GODOT_EXTENSION
#include "modules/register_module_types.h"

void initialize_luaAPI_module(ModuleInitializationLevel p_level);
void uninitialize_luaAPI_module(ModuleInitializationLevel p_level);
#else

#include <godot_cpp/core/class_db.hpp>
using namespace godot;

void initialize_luaAPI_types(ModuleInitializationLevel p_level);
void uninitialize_luaAPI_types(ModuleInitializationLevel p_level);
#endif

#endif