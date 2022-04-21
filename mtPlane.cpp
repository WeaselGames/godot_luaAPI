#include "lua.h"

static std::map<String, lua_CFunction> planeFuncs;

// Create metatable for Color and saves it at LUA_REGISTRYINDEX with name "mt_Color"
void Lua::createPlaneMetatable( ){
 
    {
        planeFuncs["normalized"] = LUA_LAMBDA_TEMPLATE({
            Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
            lua->pushVariant((var->operator Plane()).normalized());                           
            return 1;
        });
        // TODO: Add rest of funcs
    }

    luaL_newmetatable( state , "mt_Plane" );
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__index" , {
        Variant value = arg1.get( arg2 );
        if (!value.is_null()) {
            lua->pushVariant( value ) ;
            return 1;
        }
        
        // Index was not found, so check to see if there is a matching function
        if (planeFuncs.find(arg2.operator String()) != planeFuncs.end()) {
            auto f = planeFuncs.at(arg2.operator String());
			// passing pointer instead of arg1, not sure if this is needed or not yet.
            lua_pushlightuserdata(inner_state, lua_touserdata(inner_state,1));
            lua_pushcclosure(inner_state, f, 1);
            return 1;
        }
        
        return 0;
    });
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__newindex" , {
        // We can't use arg1 here because we need to reference the userdata
        ((Variant*)lua_touserdata(inner_state,1))->set( arg2 , arg3 );
        return 0;
    }); 
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__eq" , {
        lua->pushVariant( arg1.operator Plane() == arg2.operator Plane() );
        return 1;
    });
    
 
    lua_pop(state,1); // Stack is now unmodified
}