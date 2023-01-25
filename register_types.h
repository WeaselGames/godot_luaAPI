#ifndef LUA_REGISTER_TYPES_H
#define LUA_REGISTER_TYPES_H
#include "modules/register_module_types.h"


#ifndef LAPI_GODOT_EXTENSION
void initialize_luaAPI_module(ModuleInitializationLevel p_level);
void uninitialize_luaAPI_module(ModuleInitializationLevel p_level);
#else
using namespace godot;

void initialize_luaAPI_types(ModuleInitializationLevel p_level);
void uninitialize_luaAPI_types(ModuleInitializationLevel p_level);
#endif