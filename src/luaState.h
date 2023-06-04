#ifndef LUASTATE_H
#define LUASTATE_H


#ifndef LAPI_GDEXTENSION
#include "core/object/ref_counted.h"
#include "core/variant/callable.h"
#else
#include <godot_cpp/classes/ref.hpp>
#include <classes/luaCallable.h>
#include <godot_cpp/templates/vmap.hpp>
#endif

#include <lua/lua.hpp>
#include <classes/luaError.h>

class LuaState {
    public:
        void setState(lua_State* state, RefCounted* obj, bool bindAPI);
        void bindLibraries(Array libs);

        void setHook(Callable hook, int mask, int count);

        bool luaFunctionExists(String functionName);

        lua_State* getState() const;

        Variant getVar(int index = -1) const;
        Variant pullVariant(String name);
        Variant callFunction(String functionName, Array args);

        LuaError* pushVariant(Variant var) const;
        LuaError* pushGlobalVariant(String name, Variant var);
        LuaError* exposeObjectConstructor(String name, Object* obj);
        LuaError* handleError(int lua_error) const;

        Object* getGDFuncState();
        
        static LuaError* pushVariant(lua_State* state, Variant var);
        static LuaError* handleError(lua_State* state, int lua_error);
        #ifndef LAPI_GDEXTENSION
        static LuaError* handleError(const StringName &func, Callable::CallError error, const Variant** p_arguments, int argc);
        #else
        static LuaError* handleError(const StringName &func, GDExtensionCallError error, const Variant** p_arguments, int argc);
        #endif
        static Variant getVariant(lua_State* state, int index, const RefCounted* obj);

        // Lua functions
        static int luaErrorHandler(lua_State* state);
        static int luaPrint(lua_State* state);
        static int luaUserdataFuncCall(lua_State* state);
        static int luaCallableCall(lua_State* state);

        static void luaHook(lua_State* state, lua_Debug* ar);
    private:
        lua_State* L = nullptr;
        RefCounted* obj;

        static VMap<lua_State*, Object*> GDFunctionStates;

        void exposeConstructors();
        void createVector2Metatable();
        void createVector3Metatable();
        void createColorMetatable();
        void createRect2Metatable();
        void createPlaneMetatable();
        void createSignalMetatable();
        void createObjectMetatable();
        void createCallableMetatable();
        void createCallableExtraMetatable();
};

#endif