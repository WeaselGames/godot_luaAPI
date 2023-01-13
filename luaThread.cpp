#include "luaThread.h"
#include "lua.h"
#include "luaCallable.h"

void LuaThread::_bind_methods() {
    ClassDB::bind_static_method("LuaThread", D_METHOD("new_thread", "lua"), &LuaThread::newThread);
    
    ClassDB::bind_method(D_METHOD("bind", "lua"), &LuaThread::bind);
    ClassDB::bind_method(D_METHOD("resume"), &LuaThread::resume);
    ClassDB::bind_method(D_METHOD("load_string"), &LuaThread::loadString);
    ClassDB::bind_method(D_METHOD("load_file"), &LuaThread::loadFile);
    ClassDB::bind_method(D_METHOD("is_done"), &LuaThread::isDone);

}

LuaThread* LuaThread::newThread(Ref<Lua> lua) {
    LuaThread* thread = memnew(LuaThread);
    thread->bind(lua);
    return thread;
}

// binds the thread to a lua object
void LuaThread::bind(Ref<Lua> lua) {
    parentState = lua->getState();
    state = lua->newThread();
    
    // Updating the internal object to be this
    lua_pushstring(state, "__OBJECT");
	lua_pushlightuserdata(state, this);
    // register the yield method
    lua_register(state, "yield", luaYield);
}

// loads a string into the threads state
void LuaThread::loadString(String code) {
    done = false;
    luaL_loadstring(state, code.ascii().get_data());
}


LuaError* LuaThread::loadFile(String fileName) {
    done = false;
    Error error;
    Ref<FileAccess> file = FileAccess::open(fileName, FileAccess::READ, &error);
    if (error != Error::OK) {
        return LuaError::newErr(vformat("error '%s' while opening file '%s'", error_names[error], fileName), LuaError::ERR_FILE);
    }

    String path = file->get_path_absolute();
    int ret = luaL_loadfile(state, path.ascii().get_data());
    if (ret != LUA_OK) {
        return handleError(ret);
    }
    return nullptr;
}

// Value 1 will always be a boolean which indicates weather the thread is done or not.
// If a error occures it will be value number 2, otherwise the rest of the values are arguments passed to yield()
Variant LuaThread::resume() {
    if (done) {
        return LuaError::newErr("Thread is done executing", LuaError::ERR_RUNTIME);
    }

    int argc;
    int ret = lua_resume(state, NULL, 0, &argc);
    if (ret == LUA_OK) done = true; // thread is finished
    else if (ret != LUA_YIELD) {
        done = true;
        return handleError(ret);
    }
    
    Array toReturn;
    for (int i = 1; i <= argc; i++) {
        toReturn.append(Lua::getVariant(i, state, Ref<RefCounted>(this)));
    }

    return toReturn;
}

bool LuaThread::isDone() {
    return done;
}

LuaError* LuaThread::handleError(int lua_error) const {
    String msg;
    switch(lua_error) {
        case LUA_ERRRUN: {
            msg += "[LUA_ERRRUN - runtime error ]\n";
            luaL_traceback(parentState, state, lua_tostring(state, -1), 2);
            msg += lua_tostring(state, -1);
            msg += "\n";
            lua_pop(state, 1);
            break;
        }
        case LUA_ERRSYNTAX:{
            msg += "[LUA_ERRSYNTAX - syntax error ]\n";
            luaL_traceback(parentState, state, lua_tostring(state, -1), 2);
            msg += lua_tostring(state, -1);
            msg += "\n";
            lua_pop(state, 1);
            break;
        }
        case LUA_ERRMEM:{
            msg += "[LUA_ERRMEM - memory allocation error ]\n";
            break;
        }
        case LUA_ERRERR:{
            msg += "[LUA_ERRERR - error while handling another error]\n";
            break;
        }
        case LUA_ERRFILE:{
            msg += "[LUA_ERRFILE - error while opening file]\n";
            break;
        }
        default: break;
    }
    
    return LuaError::newErr(msg, static_cast<LuaError::ErrorType>(lua_error));
}

int LuaThread::luaYield(lua_State *state) {
    int argc = lua_gettop(state);
    return lua_yield(state, argc);
}