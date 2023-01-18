#include "luaAPI.h"
#include "luaCallable.h"

LuaAPI::LuaAPI() {
    lState = luaL_newstate();
	// Createing lua state instance
	state.setState(lState, Ref<RefCounted>(this), true);
}

LuaAPI::~LuaAPI() {
    lua_close(lState);
}

// Bind C++ functions to GDScript
void LuaAPI::_bind_methods() {
    ClassDB::bind_method(D_METHOD("do_file", "File"), &LuaAPI::doFile);
    ClassDB::bind_method(D_METHOD("do_string", "Code"), &LuaAPI::doString);

    ClassDB::bind_method(D_METHOD("bind_libs", "Array"),&LuaAPI::bindLibs);
    ClassDB::bind_method(D_METHOD("push_variant", "var", "Name"), &LuaAPI::pushGlobalVariant);
    ClassDB::bind_method(D_METHOD("pull_variant", "Name"), &LuaAPI::pullVariant);
    ClassDB::bind_method(D_METHOD("expose_constructor", "Object", "LuaConstructorName"), &LuaAPI::exposeObjectConstructor);
    ClassDB::bind_method(D_METHOD("call_function", "LuaFunctionName", "Args"), &LuaAPI::callFunction);
    ClassDB::bind_method(D_METHOD("function_exists","LuaFunctionName"), &LuaAPI::luaFunctionExists);
}

// Calls LuaState::bindLibs()
void LuaAPI::bindLibs(Array libs) {
    state.bindLibs(libs);
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
LuaError* LuaAPI::pushGlobalVariant(Variant var, String name) {
    return state.pushGlobalVariant(var, name);
}

// Calls LuaState::exposeObjectConstructor()
LuaError* LuaAPI::exposeObjectConstructor(Object* obj, String name) {
    return state.exposeObjectConstructor(obj, name);
}

// addFile() calls luaL_loadfille with the absolute file path
LuaError* LuaAPI::doFile(String fileName) {
    // push the error handler onto the stack
    lua_pushcfunction(lState, LuaState::luaErrorHandler);

    Error error;
    Ref<FileAccess> file = FileAccess::open(fileName, FileAccess::READ, &error);
    if (error != Error::OK) {
        return LuaError::newErr(vformat("error '%s' while opening file '%s'", error_names[error], fileName), LuaError::ERR_FILE);
    }

    String path = file->get_path_absolute();
    int ret = luaL_loadfile(lState, path.ascii().get_data());
    if (ret != LUA_OK) {
        return state.handleError(ret);
    }

    LuaError* err = execute(-2);
    // pop the error handler from the stack
    lua_pop(lState, 1);
    return err;
}

// Loads string into lua state and executes the top of the stack
LuaError* LuaAPI::doString(String code) {
    // push the error handler onto the stack
    lua_pushcfunction(lState, LuaState::luaErrorHandler);
    luaL_loadstring(lState, code.ascii().get_data());
    LuaError* err = execute(-2);
    // pop the error handler from the stack
    lua_pop(lState, 1);
    return err;
}

// Execute the current lua stack, return error as string if one occures, otherwise return String()
LuaError* LuaAPI::execute(int handlerIndex) {
    int ret = lua_pcall(lState, 0, 0, handlerIndex);
    if (ret != LUA_OK) {
        return state.handleError(ret);
    }
    return nullptr;
}

// Creates a new thread staee
lua_State* LuaAPI::newThread() {
    return lua_newthread(lState);
}

// returns state
lua_State* LuaAPI::getState() {
    return lState;
}