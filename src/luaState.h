#ifndef LUASTATE_H
#define LUASTATE_H

#ifndef LAPI_GODOT_EXTENSION
#include "core/object/ref_counted.h"
#include "core/variant/callable.h"
#else
#include <godot_cpp/classes/ref.hpp>
#endif

#include <lua/lua.hpp>
#include <classes/luaError.h>

#ifdef LAPI_GODOT_EXTENSION
using namespace godot;
#endif

class LuaState {
    public:
        void setState(lua_State* state, RefCounted* obj, bool bindAPI);
        void bindLibraries(Array libs);
        void pushFunction(String functionName, Callable function, int argc, bool tuple) const;

        bool luaFunctionExists(String functionName);

        lua_State* getState() const;

        Variant getVar(int index = -1) const;
        Variant pullVariant(String name);
        Variant callFunction(String functionName, Array args);

        LuaError* pushVariant(Variant var) const;
        LuaError* pushGlobalVariant(String name, Variant var);
        LuaError* exposeObjectConstructor(String name, Object* obj);
        LuaError* handleError(int lua_error) const;
        
        static LuaError* pushVariant(lua_State* state, Variant var);
        static LuaError* handleError(lua_State* state, int lua_error);
        #ifndef LAPI_GODOT_EXTENSION
        static LuaError* handleError(const StringName &func, Callable::CallError error, const Variant** p_arguments, int argc);
        #else
        static LuaError* handleError(const StringName &func, GDExtensionCallError error, const Variant** p_arguments, int argc);
        #endif

        static Variant getVariant(lua_State* state, int index, const RefCounted* obj);

        // Lua functions
        static int luaErrorHandler(lua_State* state);
        static int luaPrint(lua_State* state);
        static int luaExposedFuncCall(lua_State* state);
        static int luaUserdataFuncCall(lua_State* state);
        static int luaCallableCall(lua_State* state);
    private:
        lua_State *L = nullptr;
        RefCounted* obj;

        void exposeConstructors();
        void createVector2Metatable();
        void createVector3Metatable();
        void createColorMetatable();
        void createRect2Metatable();
        void createPlaneMetatable();
        void createObjectMetatable();
        void createCallableMetatable();
        void createCallableExtraMetatable();
};

#endif