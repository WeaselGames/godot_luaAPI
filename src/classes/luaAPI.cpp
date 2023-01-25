#include "luaAPI.h"

#ifdef LAPI_GODOT_EXTENSION
#include <godot_cpp/classes/file_access.hpp>
#endif

LuaAPI::LuaAPI() {
    lState = luaL_newstate();
	// Createing lua state instance
	state.setState(lState, this, true);
}

LuaAPI::~LuaAPI() {
    for (auto &[key, val] : ownedObjects) {
        if (val != nullptr)
            memdelete(val);
    }
    lua_close(lState);
}

// Bind C++ functions to GDScript
void LuaAPI::_bind_methods() {
    ClassDB::bind_method(D_METHOD("do_file", "FilePath"), &LuaAPI::doFile);
    ClassDB::bind_method(D_METHOD("do_string", "Code"), &LuaAPI::doString);

    ClassDB::bind_method(D_METHOD("bind_libraries", "Array"), &LuaAPI::bindLibraries);
    ClassDB::bind_method(D_METHOD("push_variant", "Name", "var"), &LuaAPI::pushGlobalVariant);
    ClassDB::bind_method(D_METHOD("pull_variant", "Name"), &LuaAPI::pullVariant);
    ClassDB::bind_method(D_METHOD("expose_constructor", "LuaConstructorName", "Object"), &LuaAPI::exposeObjectConstructor);
    ClassDB::bind_method(D_METHOD("call_function", "LuaFunctionName", "Args"), &LuaAPI::callFunction);
    ClassDB::bind_method(D_METHOD("function_exists", "LuaFunctionName"), &LuaAPI::luaFunctionExists);
}

// Calls LuaState::bindLibs()
void LuaAPI::bindLibraries(Array libs) {
    state.bindLibraries(libs);
}

// Adds the pointer to a object now owned by lua for cleanup later
void LuaAPI::addOwnedObject(void* luaPtr, Variant* obj) {
    ownedObjects[luaPtr] = obj;
}

// Adds the pointer to a object now owned by lua for cleanup later
void LuaAPI::removeOwnedObject(Variant* obj) {
    if (ownedObjects.empty() || ownedObjects.count((void*)obj)==0)
        return;
    memdelete(ownedObjects[(void*)obj]);
    ownedObjects[(void*)obj] = nullptr;
}

// Adds the pointer to a object now owned by lua for cleanup later
void LuaAPI::removeOwnedObject(void* luaPtr) {
    if (ownedObjects.empty() || ownedObjects.count(luaPtr)==0)
        return;
    memdelete(ownedObjects[luaPtr]);
    ownedObjects[luaPtr] = nullptr;
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
LuaError* LuaAPI::pushGlobalVariant(String name, Variant var) {
    return state.pushGlobalVariant(name, var);
}

// Calls LuaState::exposeObjectConstructor()
LuaError* LuaAPI::exposeObjectConstructor(String name, Object* obj) {
    return state.exposeObjectConstructor(name, obj);
}

// addFile() calls luaL_loadfille with the absolute file path
LuaError* LuaAPI::doFile(String fileName) {
    // push the error handler onto the stack
    lua_pushcfunction(lState, LuaState::luaErrorHandler);

    Error error;
    String path;
    // fileAccess never unrefs without this
    {
        #ifndef LAPI_GODOT_EXTENSION
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