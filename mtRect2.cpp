#include "lua.h"

static std::map<String, lua_CFunction> rect2Funcs;

// Create metatable for Color and saves it at LUA_REGISTRYINDEX with name "mt_Color"
void Lua::createRect2Metatable( ){
    {
        rect2Funcs["get_area"] = LUA_LAMBDA_TEMPLATE({
            Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
            lua->pushVariant((var->operator Rect2()).get_area());                           
            return 1;
        });
        // TODO: Add rest of funcs
    }

    luaL_newmetatable( state , "mt_Rect2" );
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__index" , {
        // Index was not found, so check to see if there is a matching function
        if (rect2Funcs.find(arg2.operator String()) != rect2Funcs.end()) {
            auto f = rect2Funcs.at(arg2.operator String());
			// passing pointer instead of arg1, not sure if this is needed or not yet.
            lua_pushlightuserdata(inner_state, lua_touserdata(inner_state,1));
            lua_pushcclosure(inner_state, f, 1);
            return 1;
        }
        
        lua->pushVariant( arg1.get( arg2 ) );
        return 1;
    });
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__newindex" , {
        // We can't use arg1 here because we need to reference the userdata
        ((Variant*)lua_touserdata(inner_state,1))->set( arg2 , arg3 );
        return 0;
        
    }); 
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__eq" , {
        lua->pushVariant( arg1.operator Rect2() == arg2.operator Rect2() );
        return 1;
    });
 
    lua_pop(state,1); // Stack is now unmodified
}