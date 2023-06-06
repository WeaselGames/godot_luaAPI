#include "luaCallableExtra.h"

#include "luaState.h"
#include "luaTuple.h"

#ifndef LAPI_GDEXTENSION
#include "core/variant/callable.h"
#else
#include <godot_cpp/variant/callable.hpp>
#endif

void LuaCallableExtra::_bind_methods() {
    ClassDB::bind_static_method("LuaCallableExtra", D_METHOD("with_tuple", "Callable", "argc"), &LuaCallableExtra::withTuple);
    ClassDB::bind_static_method("LuaCallableExtra", D_METHOD("with_ref", "Callable"), &LuaCallableExtra::withRef);
    ClassDB::bind_static_method("LuaCallableExtra", D_METHOD("with_ref_and_tuple", "Callable", "argc"), &LuaCallableExtra::withRefAndTuple);
    
    ClassDB::bind_method(D_METHOD("set_info", "Callable", "argc", "isTuple", "wantsRef"), &LuaCallableExtra::setInfo);

    ClassDB::bind_method(D_METHOD("set_tuple", "isTuple"), &LuaCallableExtra::setTuple);
    ClassDB::bind_method(D_METHOD("get_tuple"), &LuaCallableExtra::getTuple);

    ClassDB::bind_method(D_METHOD("set_wants_ref", "wantsRef"), &LuaCallableExtra::setWantsRef);
    ClassDB::bind_method(D_METHOD("get_wants_ref"), &LuaCallableExtra::getWantsRef);

    ClassDB::bind_method(D_METHOD("set_argc", "argc"), &LuaCallableExtra::setArgc);
    ClassDB::bind_method(D_METHOD("get_argc"), &LuaCallableExtra::getArgc);

    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "tuple", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_tuple", "get_tuple");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "wants_ref", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_wants_ref", "get_wants_ref");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "argc", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_argc", "get_argc");
}

void LuaCallableExtra::setInfo(Callable function, int argc, bool isTuple, bool wantsRef) {
    this->function = function;
    this->argc = argc;
    this->isTuple = isTuple;
    this->wantsRef = wantsRef;
}

void LuaCallableExtra::setTuple(bool isTuple) {
    this->isTuple = isTuple;
}

void LuaCallableExtra::setWantsRef(bool wantsRef) {
    this->wantsRef = wantsRef;
}

void LuaCallableExtra::setArgc(int argc) {
    this->argc = argc;
}

bool LuaCallableExtra::getTuple() {
    return this->isTuple;
}

bool LuaCallableExtra::getWantsRef() {
    return this->wantsRef;
}

int LuaCallableExtra::getArgc() {
    return this->argc;
}

// Used for the __call metamethod
int LuaCallableExtra::call(lua_State *state) {
    lua_pushstring(state, "__OBJECT");
    lua_rawget(state, LUA_REGISTRYINDEX);
    RefCounted* OBJ = (RefCounted*) lua_touserdata(state, -1);
    lua_pop(state, 1);

    int l_argc = lua_gettop(state)-1; // We subtract 1 becuase the LuaCallableExtra is counted
    int noneMulty = l_argc;
    LuaCallableExtra* func = (LuaCallableExtra*) LuaState::getVariant(state, 1, OBJ).operator Object*();
    if (func == nullptr) {
        LuaError* err = LuaError::newError("Error during LuaCallableExtra::call fun==null", LuaError::ERR_RUNTIME);
        lua_pushstring(state, err->getMessage().ascii().get_data());
        lua_error(state);
        return 0;
    }
    if (func->isTuple)
        noneMulty=func->argc-1; // We subtract one becuase the tuple is countedA
    
    Array args;

    if (func->wantsRef)
        args.append(OBJ);

    int index = 2; // we start at 2 becuase the LuaCallableExtra is arg 1
    for (int i = 0; i < noneMulty; i++) {
        args.append(LuaState::getVariant(state, index++, OBJ));
    }

    if (func->isTuple) {
        Array tupleArgs;
        for (int i = noneMulty; i < l_argc; i++) {
            tupleArgs.push_back(LuaState::getVariant(state, index++, OBJ));
        }
        args.append(LuaTuple::fromArray(tupleArgs));
    }

    Vector<const Variant*> mem_args;
    mem_args.resize(args.size());
    for (int i = 0; i < args.size(); i++) {
        mem_args.write[i] = &args[i];
    }

    const Variant **p_args = (const Variant **)mem_args.ptr();

    Variant returned;
    #ifndef LAPI_GDEXTENSION
    Callable::CallError error;
    func->function.callp(p_args, args.size(), returned, error);
    if (error.error != error.CALL_OK) {
        LuaError* err = LuaState::handleError(func->function.get_method(), error, p_args, args.size());
        lua_pushstring(state, err->getMessage().ascii().get_data());
        lua_error(state);
        return 0;
    }
    #else
    returned = func->function.callv(args);
    #endif

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

LuaCallableExtra* LuaCallableExtra::withRefAndTuple(Callable func, int argc) {
    LuaCallableExtra* toReturn = memnew(LuaCallableExtra);
    toReturn->setInfo(func, argc, true, true);
    return toReturn;
}