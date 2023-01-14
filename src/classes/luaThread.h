#ifndef LUATHREAD_H
#define LUATHREAD_H

#include "luaError.h" 

#include "core/object/ref_counted.h"
#include "core/core_bind.h"

#include <src/lua.hpp>
#include <src/luaState.h>

class LuaAPI;

class LuaThread : public RefCounted {
    GDCLASS(LuaThread, RefCounted);

    protected:
        static void _bind_methods();

    public:
        static LuaThread* newThread(Ref<LuaAPI> lua);

        void bind(Ref<LuaAPI> lua);
        void loadString(String code);
        void bindLibs(Array libs);

        bool luaFunctionExists(String functionName);

        LuaError* loadFile(String fileName);
        LuaError* pushGlobalVariant(Variant var, String name);
        LuaError* exposeObjectConstructor(Object* obj, String name);

        Variant resume();
        Variant pullVariant(String name);
        Variant callFunction(String functionName, Array args);

        bool isDone();
        
        static int luaYield(lua_State *state);
    private:
        LuaState state;
        lua_State* tState;
        lua_State* parentState;
        LuaError* handleError(int lua_error) const;
        bool done;
        bool shouldCloseParent = false;
};

#endif