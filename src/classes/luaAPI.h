#ifndef LUAAPI_H
#define LUAAPI_H

#include "core/object/ref_counted.h"
#include "core/core_bind.h"
#include "luaError.h"

#include <lua/lua.hpp>
#include <luaState.h>

#include <string>
#include <map>

class LuaAPI : public RefCounted {
    GDCLASS(LuaAPI, RefCounted);

    protected:
        static void _bind_methods();

    public:
        LuaAPI();
        ~LuaAPI();

        void bindLibraries(Array libs);
        void addOwnedObject(void* luaPtr, Variant* obj);
        void removeOwnedObject(Variant* obj);
        void removeOwnedObject(void* luaPtr);

        bool luaFunctionExists(String functionName);

        Variant pullVariant(String name);
        Variant callFunction(String functionName, Array args);

        LuaError* doFile(String fileName);
        LuaError* doString(String code);
        LuaError* pushGlobalVariant(String name, Variant var);
        LuaError* exposeObjectConstructor(String name, Object* obj);
        
        lua_State* newThread();
        lua_State* getState();

        inline void addRef(Variant var) {
            refs.append(var);
        }

    private:
        LuaState state;
        lua_State* lState;
        // Temp. Looking for better method
        Array refs;
        std::map<void*, Variant*> ownedObjects;
        LuaError* execute(int handlerIndex);
};

#endif