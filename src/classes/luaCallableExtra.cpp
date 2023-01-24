#include "luaCallableExtra.h"

#include "luaState.h"
#include "luaTuple.h"
#include "core/variant/callable.h"

void LuaCallableExtra::_bind_methods() {
    ClassDB::bind_static_method("LuaCallableExtra", D_METHOD("with_tuple", "Callable", "argc"), &LuaCallableExtra::withTuple);
    ClassDB::bind_static_method("LuaCallableExtra", D_METHOD("with_ref", "Callable"), &LuaCallableExtra::withRef);
    ClassDB::bind_method(D_METHOD("set_info", "Callable", "argc", "isTuple", "wantsRef"), &LuaCallableExtra::setInfo);
}

void LuaCallableExtra::setInfo(Callable function, int argc, bool isTuple, bool wantsRef) {
    this->function = function;
    this->argc = argc;
    this->isTuple = isTuple;
    this->wantsRef = wantsRef;
}

// Used for the __call metamethod
int LuaCallableExtra::call(lua_State *state) {
    lua_pushstring(state, "__OBJECT");
    lua_rawget(state, LUA_REGISTRYINDEX);
    RefCounted* OBJ = (RefCounted*) lua_touserdata(state, -1);
    lua_pop(state, 1);

    int argc = lua_gettop(state)-1; // We subtract 1 becuase the LuaCallableExtra is counted
    int noneMulty = argc;
    // TODO: Check if func is null
    LuaCallableExtra* func = (LuaCallableExtra*) LuaState::getVariant(state, 1, OBJ).operator Object*();
    if (func->isTuple)
        noneMulty=func->argc-1; // We subtract one becuase the tuple is countedA
    
    Vector<Variant> p_args;

    if (func->wantsRef)
        p_args.append(OBJ);

    int index = 2; // we start at 2 becuase the LuaCallableExtra is arg 1
    for (int i = 0; i < noneMulty; i++) {
        p_args.append(LuaState::getVariant(state, index++, OBJ));
    }

    if (func->isTuple) {
        Array tupleArgs;
        for (int i = noneMulty; i < argc; i++) {
            tupleArgs.push_back(LuaState::getVariant(state, index++, OBJ));
        }
        p_args.append(LuaTuple::fromArray(tupleArgs));
    }

    const Variant **args = (const Variant**)alloca(sizeof(const Variant**) * p_args.size());
    for (int i = 0; i < p_args.size(); i++) {
        args[i] = &p_args[i];
    }

    Variant returned;
    Callable::CallError error;
    func->function.callp(args, p_args.size(), returned, error);
    if (error.error != error.CALL_OK) {
        LuaError* err = LuaState::handleError(func->function.get_method(), error, args, argc);
        lua_pushstring(state, err->getMessage().ascii().get_data());
        lua_error(state);
        return 0;
    }

    LuaState::pushVariant(state, returned);
    if (LuaTuple* tuple = Object::cast_to<LuaTuple>(returned.operator Object*()); tuple != nullptr)
        return tuple->size();
    return 1;
}

LuaCallableExtra* LuaCallableExtra::withTuple(Callable func, int argc) {
    LuaCallableExtra* toReturn = memnew(LuaCallableExtra);
    toReturn->setInfo(func, argc, true, false);
    return toReturn;
}

LuaCallableExtra* LuaCallableExtra::withRef(Callable func) {
    LuaCallableExtra* toReturn = memnew(LuaCallableExtra);
    toReturn->setInfo(func, 0, false, true);
    return toReturn;
}