#ifndef LUASTATE_H
#define LUASTATE_H

#ifndef LAPI_GDEXTENSION
#include "core/object/ref_counted.h"
#include "core/variant/callable.h"
#else
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/templates/vmap.hpp>
#endif

#include <classes/luaError.h>
#include <lua/lua.hpp>

#define METATABLE_DISCLAIMER "This metatable is protected."

class LuaAPI;

class LuaState {
public:
	void setState(lua_State *state, LuaAPI *lua, bool bindAPI);
	void setHook(Callable hook, int mask, int count);

	bool luaFunctionExists(String functionName);

	lua_State *getState() const;

	Variant getVar(int index = -1) const;
	Variant pullVariant(String name);
	Variant callFunction(String functionName, Array args);

	Variant getRegistryValue(String name);

	Ref<LuaError> setRegistryValue(String name, Variant var);
	Ref<LuaError> bindLibraries(Array libs);
	Ref<LuaError> pushVariant(Variant var) const;
	Ref<LuaError> pushGlobalVariant(String name, Variant var);
	Ref<LuaError> handleError(int lua_error) const;

	static LuaAPI *getAPI(lua_State *state);

	static Ref<LuaError> pushVariant(lua_State *state, Variant var);
	static Ref<LuaError> handleError(lua_State *state, int lua_error);
#ifndef LAPI_GDEXTENSION
	static Ref<LuaError> handleError(const StringName &func, Callable::CallError error, const Variant **p_arguments, int argc);
#else
	static Ref<LuaError> handleError(const StringName &func, GDExtensionCallError error, const Variant **p_arguments, int argc);
#endif
	static Variant getVariant(lua_State *state, int index);

	// Lua functions
	static int luaErrorHandler(lua_State *state);
	static int luaPrint(lua_State *state);
	static int luaUserdataFuncCall(lua_State *state);
	static int luaCallableCall(lua_State *state);

	static void luaHook(lua_State *state, lua_Debug *ar);

private:
	lua_State *L = nullptr;

	// Helper functions for recursive indexing
	void indexForReading(String name); // Puts the object on the stack
	String indexForWriting(String name); // Puts the table on the stack and gives the last name. (Please make sure the table is not nil.)

	void exposeConstructors();
	void createVector2Metatable();
	void createVector3Metatable();
	void createColorMetatable();
	void createRect2Metatable();
	void createPlaneMetatable();
	void createSignalMetatable();
	void createObjectMetatable();
	void createCallableMetatable();
	void createCallableExtraMetatable();
};

#endif
