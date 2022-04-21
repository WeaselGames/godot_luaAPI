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

static std::map<void*, int> objIndex;

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

        Array allowedFields = Array();
        if(arg1.has_method("lua_fields")) {
            allowedFields = arg1.call("lua_fields");
        }
        // If the field is allowed
        if(allowedFields.is_empty() || allowedFields.has(arg2)) {
            Variant var = arg1.get(arg2);
            lua->pushVariant(var);
            return 1;
        }
        
        return 0;
    });

    // Makeing sure to clean up the pointer
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__gc" , {
        int index = objIndex[lua_touserdata(inner_state,1)];
        // Indexing by the userdata pointer to get the og pointer for cleanup
        Variant* ptr = lua->luaObjects[index];
        delete ptr;
        return 0;
    });

    lua_pop(state,1);
}

// This is the only way I found to let lua own the object
void Lua::exposeObjectConstructor(Object* obj, String name) {
    // Make sure we are able to call new
    if (!obj->has_method("new")) {
        print_error( "Error during \"Lua::exposeObjectConstructor\" method 'new' does not exist." );
        return;
    }
    lua_pushlightuserdata(state, obj);
    lua_pushcclosure(state, LUA_LAMBDA_TEMPLATE({
        Object* inner_obj = (Object*)lua_touserdata(inner_state, lua_upvalueindex(1));
        
        // We cant store the variant directly in the userdata. It will causes crashes.
        Variant* var = new Variant;
        *var = inner_obj->call("new");
        lua->luaObjects.push_back(var);
        void* userdata = (Variant*)lua_newuserdata( inner_state , sizeof(Variant) );
        objIndex[userdata] = lua->luaObjects.size()-1;
        memcpy( userdata , (void*)var , sizeof(Variant) );
        // If its owned by lua we wont to clean up the pointer.

        luaL_setmetatable(inner_state, "mt_Object");
        return 1;
    }), 1);
    lua_setglobal(state, name.ascii().get_data() );
}