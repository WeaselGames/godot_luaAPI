#ifndef LUASTATE_H
#define LUASTATE_H

#include "core/object/ref_counted.h"

#include "lua.hpp"
#include "classes/luaError.h"

class LuaState {
    public:
        void setState(lua_State* state);
        void bindLibs(Array libs);

        bool luaFunctionExists(String functionName);

        lua_State* getState() const;

        Variant getVar(int index = -1) const;
        Variant pullVariant(String name);
        Variant callFunction(String functionName, Array args);

        LuaError* pushVariant(Variant var) const;
        LuaError* pushGlobalVariant(Variant var, String name);
        LuaError* exposeObjectConstructor(Object* obj, String name);
        LuaError* handleError(int lua_error) const;
        
        static LuaError* pushVariant(Variant var, lua_State* state);
        static LuaError* handleError(int lua_error, lua_State* state);
        static LuaError* handleError(const StringName &func, Callable::CallError error, const Variant** p_arguments, int argc);

        static Variant getVariant(int index, lua_State* L, Ref<RefCounted> obj);

        // Lua functions
        static int luaErrorHandler(lua_State* state);
        static int luaPrint(lua_State* state);
        static int luaExposedFuncCall(lua_State* state);
        static int luaUserdataFuncCall(lua_State* state);
        static int luaCallableCall(lua_State* state);
    private:
        lua_State *L = nullptr;
        Ref<RefCounted> obj;

        void exposeConstructors();
        void createVector2Metatable();
        void createVector3Metatable();
        void createColorMetatable();
        void createRect2Metatable();
        void createPlaneMetatable();
        void createObjectMetatable();
        void createCallableMetatable();
};

#endif