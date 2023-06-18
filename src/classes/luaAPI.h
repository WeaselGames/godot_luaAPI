#ifndef LUAAPI_H
#define LUAAPI_H

#ifndef LAPI_GDEXTENSION
#include "core/core_bind.h"
#include "core/object/ref_counted.h"
#else
#include <godot_cpp/classes/ref.hpp>
#endif

#include "luaError.h"

#include <luaState.h>
#include <lua/lua.hpp>

#include <map>
#include <string>

#ifdef LAPI_GDEXTENSION
using namespace godot;
#endif

class LuaCoroutine;

class LuaAPI : public RefCounted {
	GDCLASS(LuaAPI, RefCounted);

protected:
	static void _bind_methods();

public:
	LuaAPI();
	~LuaAPI();

	void bindLibraries(Array libs);
	void setHook(Callable hook, int mask, int count);

	inline void setPermissive(bool permissive) {
		this->permissive = permissive;
	}

	inline bool getPermissive() const {
		return permissive;
	}

	bool luaFunctionExists(String functionName);

	Variant pullVariant(String name);
	Variant callFunction(String functionName, Array args);

	LuaError *doFile(String fileName);
	LuaError *doString(String code);
	LuaError *pushGlobalVariant(String name, Variant var);
	LuaError *exposeObjectConstructor(String name, Object *obj);

	Ref<LuaCoroutine> newCoroutine();
	Ref<LuaCoroutine> getRunningCoroutine();

	lua_State *newThreadState();
	lua_State *getState();

	inline void addRef(Variant var) {
		refs.append(var);
	}

	inline void addOwnedObject(void *luaPtr, Variant *obj) {
		ownedObjects[luaPtr] = obj;
	}

	enum HookMask {
		HOOK_MASK_CALL = LUA_MASKCALL,
		HOOK_MASK_RETURN = LUA_MASKRET,
		HOOK_MASK_LINE = LUA_MASKLINE,
		HOOK_MASK_COUNT = LUA_MASKCOUNT,
	};

private:
	LuaState state;
	lua_State *lState = nullptr;

	bool permissive = false;

	// Temp. Looking for better method. Maybe?
	Array refs;
	std::map<void *, Variant *> ownedObjects;

	LuaError *execute(int handlerIndex);
};

VARIANT_ENUM_CAST(LuaAPI::HookMask)

#endif
