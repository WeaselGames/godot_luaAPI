#ifndef LUATHREAD_H
#define LUATHREAD_H

#include "luaError.h"

#include "luasrc/lua.hpp"

#include "core/object/ref_counted.h"
#include "core/core_bind.h"
#include "luasrc/lua.hpp"

class Lua;

class LuaThread : public RefCounted {
    GDCLASS(LuaThread, RefCounted);

    protected:
        static void _bind_methods();

    public:
        static LuaThread* newThread(Ref<Lua> lua);

        void bind(Ref<Lua> lua);
        void loadString(String code);

        LuaError* loadFile(String fileName);
        Variant resume();
        bool isDone();
        
        static int luaYield(lua_State *state);

    private:
        lua_State* state;
        lua_State* parentState;
        LuaError* handleError(int lua_error) const;
        Ref<Lua> lua;
        bool done;

};

#endif