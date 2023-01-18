#ifndef LUAAPI_H
#define LUAAPI_H

#include "core/object/ref_counted.h"
#include "core/core_bind.h"
#include "luaError.h"

#include <lua/lua.hpp>
#include <luaState.h>

#include <string>
#include <vector>


class LuaAPI : public RefCounted {
    GDCLASS(LuaAPI, RefCounted);

    protected:
        static void _bind_methods();

    public:
        LuaAPI();
        ~LuaAPI();

        void bindLibs(Array libs);
        void addOwnedObject(Variant* obj);

        bool luaFunctionExists(String functionName);

        Variant pullVariant(String name);
        Variant callFunction(String functionName, Array args);

        LuaError* doFile(String fileName);
        LuaError* doString(String code);
        LuaError* pushGlobalVariant(String name, Variant var);
        LuaError* exposeObjectConstructor(String name, Object* obj);
        
        lua_State* newThread();
        lua_State* getState();

    private:
        LuaState state;
        lua_State* lState;
        std::vector<Variant*> ownedObjects;
        LuaError* execute(int handlerIndex);
};

#endif