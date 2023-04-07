#ifndef LUAAPI_H
#define LUAAPI_H

#ifndef LAPI_GDEXTENSION
#include "core/object/ref_counted.h"
#include "core/core_bind.h"
#else
#include <godot_cpp/classes/ref.hpp>
#endif

#include "luaError.h"

#include <lua/lua.hpp>
#include <luaState.h>

#include <string>
#include <map>

#ifdef LAPI_GDEXTENSION
using namespace godot;
#endif

class LuaCoroutine;

class LuaAPI : public RefCounted {
    GDCLASS(LuaAPI, RefCounted);

    protected:
        static void _bind_methods();

    public:
        LuaAPI();
        ~LuaAPI();

        void bindLibraries(Array libs);

        bool luaFunctionExists(String functionName);

        Variant pullVariant(String name);
        Variant callFunction(String functionName, Array args);

        LuaError* doFile(String fileName);
        LuaError* doString(String code);
        LuaError* pushGlobalVariant(String name, Variant var);
        LuaError* exposeObjectConstructor(String name, Object* obj);

        Ref<LuaCoroutine> newCoroutine();
        
        lua_State* newThreadState();
        lua_State* getState();

        inline void addRef(Variant var) {
            refs.append(var);
        }

        inline void addOwnedObject(void* luaPtr, Variant* obj) {
            ownedObjects[luaPtr] = obj;
        }

    private:
        LuaState state;
        lua_State* lState;

        // Temp. Looking for better method. Maybe?
        Array refs;
        std::map<void*, Variant*> ownedObjects;

        LuaError* execute(int handlerIndex);
};

#endif