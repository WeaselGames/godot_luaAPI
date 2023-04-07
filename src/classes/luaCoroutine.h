#ifndef LUACOROUTINE_H
#define LUACOROUTINE_H

#include "luaError.h" 

#ifndef LAPI_GDEXTENSION
#include "core/object/ref_counted.h"
#include "core/core_bind.h"
#else
#include <godot_cpp/classes/ref.hpp>
#endif

#include <lua/lua.hpp>
#include <luaState.h>

#ifdef LAPI_GDEXTENSION
using namespace godot;
#endif

class LuaAPI;

class LuaCoroutine : public RefCounted {
    GDCLASS(LuaCoroutine, RefCounted);

    protected:
        static void _bind_methods();

    public:
        void bind(Ref<LuaAPI> lua);
        void bindExisting(Ref<LuaAPI> lua, lua_State* tState);
        void loadString(String code);
        Signal yieldAwait(Array args);

        bool luaFunctionExists(String functionName);

        LuaError* loadFile(String fileName);
        LuaError* pushGlobalVariant(String name, Variant var);

        Ref<LuaAPI> getParent();

        Variant resume();
        Variant pullVariant(String name);
        Variant callFunction(String functionName, Array args);

        bool isDone();
        
        static int luaYield(lua_State *state);

        inline lua_State* getLuaState() {
            return tState;
        }

    private:
        LuaState state;
        Ref<LuaAPI> parent;
        lua_State* tState;
        bool done;
};

#endif