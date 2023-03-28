#ifndef LUACALLABLEEXTRA_H
#define LUACALLABLEEXTRA_H

#ifndef LAPI_GDEXTENSION
#include "core/object/ref_counted.h"
#include "core/core_bind.h"
#else
#include <godot_cpp/classes/ref.hpp>
#endif

#include <lua/lua.hpp>

#ifdef LAPI_GDEXTENSION
using namespace godot;
#endif

class LuaCallableExtra : public RefCounted {
    GDCLASS(LuaCallableExtra, RefCounted);

    protected:
        static void _bind_methods();

    public:
        static LuaCallableExtra* withTuple(Callable function, int argc);
        static LuaCallableExtra* withRef(Callable function);
        static LuaCallableExtra* withRefAndTuple(Callable function, int argc);

        void setInfo(Callable function, int argc, bool isTuple, bool wantsRef);

        void setTuple(bool isTuple);
        bool getTuple();

        void setWantsRef(bool wantsRef);
        bool getWantsRef();

        void setArgc(int argc);
        int getArgc();

        static int call(lua_State *state);

    private:
        bool isTuple = false;
        bool wantsRef = false;
        int argc = 0;

        Callable function;
};
#endif