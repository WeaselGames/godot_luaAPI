#include "lua.h"

static std::map<String, lua_CFunction> colorFuncs;

// Create metatable for Color and saves it at LUA_REGISTRYINDEX with name "mt_Color"
void Lua::createColorMetatable( ){
    {
        colorFuncs["get_h"] = LUA_LAMBDA_TEMPLATE({
            Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
            lua->pushVariant((var->operator Color()).get_h());                           
            return 1;
        });
        // TODO: Add rest of funcs
    }

    luaL_newmetatable( state , "mt_Color" );
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__index" , {
        // Index was not found, so check to see if there is a matching function
        if (colorFuncs.find(arg2.operator String()) != colorFuncs.end()) {
            auto f = colorFuncs.at(arg2.operator String());
			// passing pointer instead of arg1, not sure if this is needed or not yet.
            lua_pushlightuserdata(inner_state, lua_touserdata(inner_state,1));
            lua_pushcclosure(inner_state, f, 1);
            return 1;
        }
        
        lua->pushVariant( arg1.get( arg2 ) ) ;
        return 1;
    });
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__newindex" , {
        // We can't use arg1 here because we need to reference the userdata
        ((Variant*)lua_touserdata(inner_state,1))->set( arg2 , arg3 );
        return 0;
    }); 
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__add" , {
        lua->pushVariant( arg1.operator Color() + arg2.operator Color() );
    return 1;
    });
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__sub" , {
        lua->pushVariant( arg1.operator Color() - arg2.operator Color() );
    return 1;
    });
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__mul" , {
        switch( arg2.get_type() ){
            case Variant::Type::COLOR:
                lua->pushVariant( arg1.operator Color() * arg2.operator Color() );
                return 1;
            case Variant::Type::INT:
            case Variant::Type::FLOAT:
                lua->pushVariant( arg1.operator Color() * arg2.operator double() );
                return 1;
            default:
                return 0;
        }
    });
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__div" , {
        switch( arg2.get_type() ){
            case Variant::Type::COLOR:
                lua->pushVariant( arg1.operator Color() / arg2.operator Color() );
                return 1;
            case Variant::Type::INT:
            case Variant::Type::FLOAT:
                lua->pushVariant( arg1.operator Color() / arg2.operator double() );
                return 1;
            default:
                return 0;
        }
    });
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__eq" , {
        lua->pushVariant( arg1.operator Color() == arg2.operator Color() );
        return 1;
    });
 
    lua_pop(state,1); // Stack is now unmodified
}