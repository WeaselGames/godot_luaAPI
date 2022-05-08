#ifndef LUA_REGISTER_TYPES_H
#define LUA_REGISTER_TYPES_H
#include "modules/register_module_types.h"

void initialize_lua_module(ModuleInitializationLevel p_level);
void uninitialize_lua_module(ModuleInitializationLevel p_level);
#endif