#include "lua.h"

static int objFuncCall(lua_State *L) {
    lua_pushstring(L,"__Lua");
    lua_rawget(L,LUA_REGISTRYINDEX);
    Lua* lua = (Lua*) lua_touserdata(L,-1);
    lua_pop(L,1);
    int argc = lua_gettop(L);

    Variant arg1 = lua->getVariant(1);
    Variant arg2 = lua->getVariant(2);
    Variant arg3 = lua->getVariant(3);
    Variant arg4 = lua->getVariant(4);
    Variant arg5 = lua->getVariant(5);
    Variant* obj  = (Variant*)lua_touserdata(L, lua_upvalueindex(1));
    String fName = lua->getVariant(lua_upvalueindex(2));
    Variant ret;
    // This feels wrong but cant think of a better way atm. Passing the wrong number of args causes a error.
    switch (argc) {
        case 0:{
            ret = obj->call(fName.ascii().get_data());
            break;
        }
        case 1: {
            ret = obj->call(fName.ascii().get_data(), arg1);
            break;
        }
        case 2: {
            ret = obj->call(fName.ascii().get_data(), arg1, arg2);
            break;
        }
        case 3: {
            ret = obj->call(fName.ascii().get_data(), arg1, arg2, arg3);
            break;
        }
        case 4: {
            ret = obj->call(fName.ascii().get_data(), arg1, arg2, arg3, arg4);
            break;
        }
        case 5: {
            ret = obj->call(fName.ascii().get_data(), arg1, arg2, arg3, arg4, arg5);
            break;
        }
    }
    
    lua->pushVariant(ret);
    return 1;
}

void Lua::createObjectMetatable( ){
    luaL_newmetatable( state , "mt_Object" );

    Variant arg9;

    LUA_METAMETHOD_TEMPLATE( state , -1 , "__index" , {
        Array allowedFuncs = Array();
        if(arg1.has_method("lua_funcs")) {
            allowedFuncs = arg1.call("lua_funcs");
        }

        // If the functions is allowed and exists 
        if((allowedFuncs.is_empty() || allowedFuncs.has(arg2)) && arg1.has_method(arg2)) {
            lua_pushlightuserdata(inner_state, lua_touserdata(inner_state, 1));
            lua->pushVariant(arg2);
            lua_pushcclosure(inner_state, objFuncCall, 2);
            return 1;
        }
        
        return 0;
    });
    lua_pop(state,1);
}