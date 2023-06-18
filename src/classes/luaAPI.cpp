#include "luaAPI.h"

#include "luaCoroutine.h"

#ifdef LAPI_GDEXTENSION
#include <godot_cpp/classes/file_access.hpp>
#endif

LuaAPI::LuaAPI() {
	lState = luaL_newstate();
	// Creating lua state instance
	state.setState(lState, this, true);
	lua_setwarnf(lState, LuaAPI::default_warnf, this);
}

LuaAPI::~LuaAPI() {
	for (auto &[key, val] : ownedObjects) {
		if (val != nullptr) {
			memdelete(val);
		}
	}
	lua_close(lState);
}

// Bind C++ functions to GDScript
void LuaAPI::_bind_methods() {
	ClassDB::bind_method(D_METHOD("do_file", "FilePath"), &LuaAPI::doFile);
	ClassDB::bind_method(D_METHOD("do_string", "Code"), &LuaAPI::doString);

	ClassDB::bind_method(D_METHOD("bind_libraries", "Array"), &LuaAPI::bindLibraries);
	ClassDB::bind_method(D_METHOD("set_hook", "Hook", "HookMask", "Count"), &LuaAPI::setHook);
	ClassDB::bind_method(D_METHOD("push_variant", "Name", "var"), &LuaAPI::pushGlobalVariant);
	ClassDB::bind_method(D_METHOD("pull_variant", "Name"), &LuaAPI::pullVariant);
	ClassDB::bind_method(D_METHOD("expose_constructor", "LuaConstructorName", "Object"), &LuaAPI::exposeObjectConstructor);
	ClassDB::bind_method(D_METHOD("call_function", "LuaFunctionName", "Args"), &LuaAPI::callFunction);
	ClassDB::bind_method(D_METHOD("function_exists", "LuaFunctionName"), &LuaAPI::luaFunctionExists);

	ClassDB::bind_method(D_METHOD("new_coroutine"), &LuaAPI::newCoroutine);
	ClassDB::bind_method(D_METHOD("get_running_coroutine"), &LuaAPI::getRunningCoroutine);

	ClassDB::bind_method(D_METHOD("set_permissive", "value"), &LuaAPI::setPermissive);
	ClassDB::bind_method(D_METHOD("get_permissive"), &LuaAPI::getPermissive);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "permissive"), "set_permissive", "get_permissive");

	BIND_ENUM_CONSTANT(HOOK_MASK_CALL);
	BIND_ENUM_CONSTANT(HOOK_MASK_RETURN);
	BIND_ENUM_CONSTANT(HOOK_MASK_LINE);
	BIND_ENUM_CONSTANT(HOOK_MASK_COUNT);
}

// Calls LuaState::bindLibs()
void LuaAPI::bindLibraries(Array libs) {
	state.bindLibraries(libs);
}

void LuaAPI::setHook(Callable hook, int mask, int count) {
	return state.setHook(hook, mask, count);
}

// Calls LuaState::luaFunctionExists()
bool LuaAPI::luaFunctionExists(String functionName) {
	return state.luaFunctionExists(functionName);
}

// Calls LuaState::pullVariant()
Variant LuaAPI::pullVariant(String name) {
	return state.pullVariant(name);
}

// Calls LuaState::callFunction()
Variant LuaAPI::callFunction(String functionName, Array args) {
	return state.callFunction(functionName, args);
}

// Calls LuaState::pushGlobalVariant()
LuaError *LuaAPI::pushGlobalVariant(String name, Variant var) {
	return state.pushGlobalVariant(name, var);
}

// Calls LuaState::exposeObjectConstructor()
LuaError *LuaAPI::exposeObjectConstructor(String name, Object *obj) {
	return state.exposeObjectConstructor(name, obj);
}

// addFile() calls luaL_loadfille with the absolute file path
LuaError *LuaAPI::doFile(String fileName) {
	// push the error handler onto the stack
	lua_pushcfunction(lState, LuaState::luaErrorHandler);

	String path;
	// fileAccess never unrefs without this
	{
#ifndef LAPI_GDEXTENSION
		Error error;
		Ref<FileAccess> file = FileAccess::open(fileName, FileAccess::READ, &error);
		if (error != Error::OK) {
			return LuaError::newError(vformat("error '%s' while opening file '%s'", error_names[error], fileName), LuaError::ERR_FILE);
		}
#else
		Ref<FileAccess> file = FileAccess::open(fileName, FileAccess::READ);
		if (!file.is_valid()) {
			return LuaError::newError(vformat("error while opening file '%s'", fileName), LuaError::ERR_FILE);
		}
#endif

		path = file->get_path_absolute();
	}

	int ret = luaL_loadfile(lState, path.ascii().get_data());
	if (ret != LUA_OK) {
		return state.handleError(ret);
	}

	LuaError *err = execute(-2);
	// pop the error handler from the stack
	lua_pop(lState, 1);
	return err;
}

// Loads string into lua state and executes the top of the stack
LuaError *LuaAPI::doString(String code) {
	// push the error handler onto the stack
	lua_pushcfunction(lState, LuaState::luaErrorHandler);
	int ret = luaL_loadstring(lState, code.ascii().get_data());
	if (ret != LUA_OK) {
		return state.handleError(ret);
	}

	LuaError *err = execute(-2);
	// pop the error handler from the stack
	lua_pop(lState, 1);
	return err;
}

// Execute the current lua stack, return error as string if one occurs, otherwise return String()
LuaError *LuaAPI::execute(int handlerIndex) {
	int ret = lua_pcall(lState, 0, 0, handlerIndex);
	if (ret != LUA_OK) {
		return state.handleError(ret);
	}
	return nullptr;
}

Ref<LuaCoroutine> LuaAPI::newCoroutine() {
	Ref<LuaCoroutine> thread;
	thread.instantiate();
	thread->bind(this);
	return thread;
}

Ref<LuaCoroutine> LuaAPI::getRunningCoroutine() {
	Variant top = state.getVar();
	if (top.get_type() != Variant::Type::OBJECT) {
		return nullptr;
	}

#ifndef LAPI_GDEXTENSION
	Ref<LuaCoroutine> thread = Object::cast_to<LuaCoroutine>(top);
#else
	Ref<LuaCoroutine> thread = dynamic_cast<LuaCoroutine *>(top.operator Object *());
#endif
	return thread;
}

// Creates a new thread staee
lua_State *LuaAPI::newThreadState() {
	return lua_newthread(lState);
}

// returns state
lua_State *LuaAPI::getState() {
	return lState;
}

void LuaAPI::default_warnf(void *inst, const char *msg, int tocont) {
	LuaAPI *instance;
	instance->warn_str += msg;
	if (tocont > 0) {
		WARN_PRINT(instance->warn_str);
		instance->warn_str = String();
	}
}

void LuaAPI::setWarnFunction(Callable c) {
	warnf = c;
	state.triggerChangedWarnFunction(this);
}