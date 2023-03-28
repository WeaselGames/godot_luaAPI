#ifndef LUATHREAD_H
#define LUATHREAD_H

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

class LuaThread : public RefCounted {
    GDCLASS(LuaThread, RefCounted);

    protected:
        static void _bind_methods();

    public:
        static LuaThread* newThread(Ref<LuaAPI> lua);

        void bind(Ref<LuaAPI> lua);
        void loadString(String code);

        bool luaFunctionExists(String functionName);

        LuaError* loadFile(String fileName);
        LuaError* pushGlobalVariant(String name, Variant var);
        LuaError* exposeObjectConstructor(String name, Object* obj);

        Variant resume();
        Variant pullVariant(String name);
        Variant callFunction(String functionName, Array args);

        bool isDone();
        
        static int luaYield(lua_State *state);
    private:
        LuaState state;
        Ref<LuaAPI> parent;
        lua_State* tState;
        lua_State* parentState;
        bool done;
};

#endif