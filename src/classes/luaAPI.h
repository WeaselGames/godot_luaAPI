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

	void setPermissive(bool value);
	bool getPermissive() const;

	void setMemoryLimit(int limit);
	int getMemoryLimit() const;

	int configureGC(int what, int data);
	int getMemoryUsage() const;

	bool luaFunctionExists(String functionName);

	Variant pullVariant(String name);
	Variant callFunction(String functionName, Array args);
	Variant callFunctionRef(Array args, int funcRef);

	LuaError *doFile(String fileName);
	LuaError *doString(String code);
	LuaError *pushGlobalVariant(String name, Variant var);
	LuaError *exposeObjectConstructor(String name, Object *obj);

	Ref<LuaCoroutine> newCoroutine();
	Ref<LuaCoroutine> getRunningCoroutine();

	lua_State *newThreadState();
	lua_State *getState();

	enum HookMask {
		HOOK_MASK_CALL = LUA_MASKCALL,
		HOOK_MASK_RETURN = LUA_MASKRET,
		HOOK_MASK_LINE = LUA_MASKLINE,
		HOOK_MASK_COUNT = LUA_MASKCOUNT,
	};

	enum GCOption {
		GC_STOP = LUA_GCSTOP,
		GC_RESTART = LUA_GCRESTART,
		GC_COLLECT = LUA_GCCOLLECT,
		GC_COUNT = LUA_GCCOUNT,
		GC_COUNTB = LUA_GCCOUNTB,
		GC_STEP = LUA_GCSTEP,
		GC_SETPAUSE = LUA_GCSETPAUSE,
		GC_SETSTEPMUL = LUA_GCSETSTEPMUL,
	};

private:
	LuaState state;
	lua_State *lState = nullptr;

	static void *luaAlloc(void *ud, void *ptr, size_t osize, size_t nsize);

	struct LuaAllocData {
		int memoryUsed = 0;
		int memoryLimit = 0;
	};

	LuaAllocData luaAllocData;

	bool permissive = true;

	LuaError *execute(int handlerIndex);
};

VARIANT_ENUM_CAST(LuaAPI::HookMask)
VARIANT_ENUM_CAST(LuaAPI::GCOption)

#endif
