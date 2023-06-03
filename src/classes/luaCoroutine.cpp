#include "luaCoroutine.h"

#include "lua/lua.h"
#include "luaAPI.h"
#include "luaTuple.h"

#ifdef LAPI_GDEXTENSION
#include <godot_cpp/classes/file_access.hpp>
#endif

void LuaCoroutine::_bind_methods() {
    ClassDB::bind_method(D_METHOD("bind", "lua"), &LuaCoroutine::bind);
    ClassDB::bind_method(D_METHOD("resume"), &LuaCoroutine::resume);
    ClassDB::bind_method(D_METHOD("yield_await", "signal"), &LuaCoroutine::yieldAwait);

    ClassDB::bind_method(D_METHOD("load_string", "Code"), &LuaCoroutine::loadString);
    ClassDB::bind_method(D_METHOD("load_file", "FilePath"), &LuaCoroutine::loadFile);
    
    ClassDB::bind_method(D_METHOD("is_done"), &LuaCoroutine::isDone);
    ClassDB::bind_method(D_METHOD("is_running"), &LuaCoroutine::isRunning);

    ClassDB::bind_method(D_METHOD("call_function", "LuaFunctionName", "Args"), &LuaCoroutine::callFunction);
    ClassDB::bind_method(D_METHOD("function_exists","LuaFunctionName"), &LuaCoroutine::luaFunctionExists);
    ClassDB::bind_method(D_METHOD("push_variant", "Name", "var"), &LuaCoroutine::pushGlobalVariant);
    ClassDB::bind_method(D_METHOD("pull_variant", "Name"), &LuaCoroutine::pullVariant);
    
    ClassDB::bind_method(D_METHOD("pause_execution"), &LuaCoroutine::pause_execution);
    ClassDB::bind_method(D_METHOD("interrupt_execution"), &LuaCoroutine::interrupt_execution);
    ClassDB::bind_method(D_METHOD("kill"), &LuaCoroutine::kill);
    
    // This is a dummy signal never meant to actually be emited. Await needs with a coroutine or a signal to work. Even though we resume it via the GDScriptFunctionState
    ADD_SIGNAL(MethodInfo("coroutine_resume"));


}

Signal LuaCoroutine::yieldAwait(Array args) {
    lua_pop(tState, 1); // Pop function off top of stack.
    for (int i = 0; i < args.size(); i++) {
        LuaError* err = state.pushVariant(args[i]);
        if (err != nullptr) {
            // TODO: Handle error
        }
    }
    return Signal(this, "coroutine_resume");
}

// Calls LuaState::luaFunctionExists()
bool LuaCoroutine::luaFunctionExists(String functionName) {
    return state.luaFunctionExists(functionName);
}

// Calls LuaState::pullVariant()
Variant LuaCoroutine::pullVariant(String name) {
    return state.pullVariant(name);
}

// Calls LuaState::pushGlobalVariant()
LuaError* LuaCoroutine::pushGlobalVariant(String name, Variant var) {
    return state.pushGlobalVariant(name, var);
}

// Calls LuaState::callFunction()
Variant LuaCoroutine::callFunction(String functionName, Array args) {
    return state.callFunction(functionName, args);
}

// binds the thread to a lua object
void LuaCoroutine::bind(Ref<LuaAPI> lua) {
    parent = lua;
    tState = lua->newThreadState();
    state.setState(tState, this, false);
    
    // register the yield method
    lua_register(tState, "yield", luaYield);
}

// binds the thread to a lua object
void LuaCoroutine::bindExisting(Ref<LuaAPI> lua, lua_State* tState) {
    done = false;
    parent = lua;
    this->tState = tState;
    state.setState(tState, this, false);
    
    // register the yield method
    lua_register(tState, "yield", luaYield);
}

Ref<LuaAPI> LuaCoroutine::getParent() { 
    return parent; 
}

// loads a string into the threads state
LuaError* LuaCoroutine::loadString(String code) {
    done = false;
    int ret = luaL_loadstring(tState, code.ascii().get_data());
    if (ret != LUA_OK) {
        return state.handleError(ret);
    }
    return nullptr;
}


LuaError* LuaCoroutine::loadFile(String fileName) {
    #ifndef LAPI_GDEXTENSION
    done = false;
    Error error;
    Ref<FileAccess> file = FileAccess::open(fileName, FileAccess::READ, &error);
    if (error != Error::OK) {
        return LuaError::newError(vformat("error '%s' while opening file '%s'", error_names[error], fileName), LuaError::ERR_FILE);
    }
    #else
    done = false;
    Ref<FileAccess> file = FileAccess::open(fileName, FileAccess::READ);
    if (file.is_valid()) {
        return LuaError::newError(vformat("error while opening file '%s'", fileName), LuaError::ERR_FILE);
    }
    #endif

    String path = file->get_path_absolute();
    int ret = luaL_loadfile(tState, path.ascii().get_data());
    if (ret != LUA_OK) {
        return state.handleError(ret);
    }
    return nullptr;
}

// Value 1 will always be a boolean which indicates weather the thread is done or not.
// If a error occures it will be value number 2, otherwise the rest of the values are arguments passed to yield()
Variant LuaCoroutine::resume() {
    if (done) {
        return LuaError::newError("Thread is done executing", LuaError::ERR_RUNTIME);
    }
    int argc;
    int ret = 0;
    Object* funcState = state.getGDFuncState();
    if (funcState != nullptr) {
        #ifndef LAPI_GDEXTENSION
        Callable::CallError error;
        Variant toReturn = funcState->callp("resume", nullptr, 0, error);
        if (error.error != error.CALL_OK)
            return LuaState::handleError("resume", error, nullptr, 0);
        #else
        Variant toReturn = funcState->callv("resume", Array());
        #endif

        state.pushVariant(toReturn);
        int retArgc = 1;
        if (toReturn.get_type() == Variant::OBJECT) {
            if (LuaTuple* tuple = Object::cast_to<LuaTuple>(toReturn.operator Object*()); tuple != nullptr)
                retArgc = tuple->size();
        }

        #ifndef LAPI_LUAJIT
        ret = lua_resume(tState, NULL, retArgc, &argc);
        #else
        ret = lua_resume(tState, retArgc);
        #endif

    }
    else {
        #ifndef LAPI_LUAJIT
        ret = lua_resume(tState, NULL, 0, &argc);
        #else
        ret = lua_resume(tState, 0);
        #endif
    }
    
    if (killing) {
        killing = false;
        lua_sethook(tState, nullptr, NULL, NULL); // clean up after thread killed
    }
    
    if (ret == LUA_OK) done = true; // thread is finished
    else if (ret != LUA_YIELD) {
        done = true;
        return state.handleError(ret);
    }

    #ifdef LAPI_LUAJIT
    argc = lua_gettop(tState);
    #endif
    
    Array toReturn;
    for (int i = 1; i <= argc; i++) {
        toReturn.append(LuaState::getVariant(tState, i, this));
    }

    return toReturn;
}

bool LuaCoroutine::isDone() {
    return done;
}

int LuaCoroutine::luaYield(lua_State *state) {
    int argc = lua_gettop(state);
    return lua_yield(state, argc);
}

void LuaCoroutine::pause_execution() {
    if (isRunning()) lua_sethook(tState, pause_hook, LUA_MASKCOUNT, 1); // force it to run next time
}

void LuaCoroutine::pause_hook(lua_State* tState,lua_Debug* dbg) {
   lua_sethook(tState, nullptr, NULL, NULL); // remove hook
   lua_yield(tState, 0);
}

void LuaCoroutine::interrupt_execution() {
    if (isRunning()) lua_sethook(tState, interrupt_hook, LUA_MASKCOUNT, 1); // force it to run next time
}

void LuaCoroutine::interrupt_hook(lua_State* tState,lua_Debug* dbg) {
   lua_sethook(tState, nullptr, NULL, NULL); // remove hook
   lua_pushstring(tState, "execution interrupted");
   lua_error(tState);
}

void LuaCoroutine::kill() {
    if (isRunning()) {
        lua_sethook(tState, terminate_hook, LUA_MASKCOUNT, 1); // force it to run next time
        killing = true;
    }
}

void LuaCoroutine::terminate_hook(lua_State* tState, lua_Debug* dbg) {
   lua_pushstring(tState, "execution terminated");
   lua_error(tState);
}

bool LuaCoroutine::isRunning() {
    return (not done) and lua_status(tState) == LUA_OK;
}
