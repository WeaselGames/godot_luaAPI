#ifndef LUATHREAD_H
#define LUATHREAD_H

#include "luaError.h" 

#include "core/object/ref_counted.h"
#include "core/core_bind.h"

#include "../lua.hpp"
#include "../luaState.h"

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

        Variant resume();
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