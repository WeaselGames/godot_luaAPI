#include "lua.h"
#include <map>
 
 
// These 2 macros helps us in constructing general metamethods.
// We can use "lua" as a "Lua" pointer and arg1, arg2, ..., arg5 as Variants objects
// Check examples in createVector2Metatable
#define LUA_LAMBDA_TEMPLATE(_f_) \
 [](lua_State* inner_state) -> int {                      \
     lua_pushstring(inner_state,"__Lua");                 \
     lua_rawget(inner_state,LUA_REGISTRYINDEX);           \
     Lua* lua = (Lua*) lua_touserdata(inner_state,-1);;   \
     lua_pop(inner_state,1);                              \
     Variant arg1 = lua->getVariant(1);                             \
     Variant arg2 = lua->getVariant(2);                             \
     Variant arg3 = lua->getVariant(3);                             \
     Variant arg4 = lua->getVariant(4);                             \
     Variant arg5 = lua->getVariant(5);                             \
     _f_                                                            \
}
 
#define LUA_METAMETHOD_TEMPLATE( lua_state , metatable_index , metamethod_name , _f_ )\
lua_pushstring(lua_state,metamethod_name); \
lua_pushcfunction(lua_state,LUA_LAMBDA_TEMPLATE( _f_ )); \
lua_settable(lua_state,metatable_index-2);
 
void Lua::exposeConstructors( ){
    
    lua_pushcfunction(state,LUA_LAMBDA_TEMPLATE({
        int argc = lua_gettop(inner_state);
        if( argc == 0 ){
            lua->pushVariant( Vector2() );
        } else {
            lua->pushVariant( Vector2( arg1.operator double() , arg2.operator double() ) );
        }
        return 1;
    }));
    lua_setglobal(state, "Vector2" );
 
    lua_pushcfunction(state,LUA_LAMBDA_TEMPLATE({
        int argc = lua_gettop(inner_state);
        if( argc == 0 ){
            lua->pushVariant( Vector3() );
        } else {
            lua->pushVariant( Vector3( arg1.operator double() , arg2.operator double() , arg3.operator double() ) );
        }
        return 1;
    }));
    lua_setglobal(state, "Vector3" );
 
    lua_pushcfunction(state,LUA_LAMBDA_TEMPLATE({
        int argc = lua_gettop(inner_state);
        if( argc == 3 ){
            lua->pushVariant( Color( arg1.operator double() , arg2.operator double() , arg3.operator double() ) );
        } else if ( argc == 4 ) {
            lua->pushVariant( Color( arg1.operator double() , arg2.operator double() , arg3.operator double() , arg4.operator double() ) );
        } else {
            lua->pushVariant( Color() );
        }
        return 1;
    }));
    lua_setglobal(state, "Color" );
 
    lua_pushcfunction(state,LUA_LAMBDA_TEMPLATE({
        int argc = lua_gettop(inner_state);
        if( argc == 2 ){
            lua->pushVariant( Rect2( arg1.operator Vector2() , arg2.operator Vector2()) );
        } else if ( argc == 4 ) {
            lua->pushVariant( Rect2( arg1.operator double() , arg2.operator double() , arg3.operator double() , arg4.operator double() ) );
        } else {
            lua->pushVariant( Rect2() );
        }
        return 1;
    }));
    lua_setglobal(state, "Rect2" );
 
    lua_pushcfunction(state,LUA_LAMBDA_TEMPLATE({
        int argc = lua_gettop(inner_state);
        if( argc == 4 ){
            lua->pushVariant( Plane( arg1.operator double() , arg2.operator double(), arg3.operator double(), arg4.operator double()) );
        } else if ( argc == 3 ) {
            lua->pushVariant( Plane( arg1.operator Vector3() , arg2.operator Vector3() , arg3.operator Vector3() ) );
        } else {
            lua->pushVariant( Plane( arg1.operator Vector3() , arg1.operator double()) );
        }
        return 1;
    }));
    lua_setglobal(state, "Plane" );
    
}

static std::map<String, lua_CFunction> vec2Funcs;
 
// Create metatable for Vector2 and saves it at LUA_REGISTRYINDEX with name "mt_Vector2"
void Lua::createVector2Metatable( ){
    luaL_newmetatable( state , "mt_Vector2" );

	vec2Funcs["normalized"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).normalized());                           
        return 1;
    });

	vec2Funcs["is_normalized"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).is_normalized());                           
        return 1;
    });

	vec2Funcs["length"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).length());                           
        return 1;
    });

	vec2Funcs["length_squared"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).length_squared());                           
        return 1;
    });

	vec2Funcs["limit_length"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
		if (!arg1.is_null()) lua->pushVariant((var->operator Vector2()).limit_length(arg1)); 
		else lua->pushVariant((var->operator Vector2()).limit_length());                           
        return 1;
    });

	vec2Funcs["min"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).min(arg1));                           
        return 1;
    });

	vec2Funcs["max"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).max(arg1));                           
        return 1;
    });

	vec2Funcs["distance_to"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).distance_to(arg1));                           
        return 1;
    });

	vec2Funcs["distance_squared_to"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).distance_squared_to(arg1));                           
        return 1;
    });

	vec2Funcs["angle_to"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).angle_to(arg1));                           
        return 1;
    });

	vec2Funcs["angle_to_point"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).angle_to_point(arg1));                           
        return 1;
    });

	vec2Funcs["direction_to"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).direction_to(arg1));                           
        return 1;
    });

	vec2Funcs["dot"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).dot(arg1));                           
        return 1;
    });

	vec2Funcs["cross"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).cross(arg1));                           
        return 1;
    });

	vec2Funcs["posmod"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).posmod(arg1));                           
        return 1;
    });

	vec2Funcs["posmodv"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).posmodv(arg1));                           
        return 1;
    });

	vec2Funcs["project"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).project(arg1));                           
        return 1;
    });

	vec2Funcs["plane_project"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).plane_project(arg1, arg2));                           
        return 1;
    });

	vec2Funcs["lerp"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).lerp(arg1, arg2));                           
        return 1;
    });

	vec2Funcs["slerp"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).slerp(arg1, arg2));                           
        return 1;
    });

	vec2Funcs["cubic_interpolate"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).cubic_interpolate(arg1, arg2, arg3, arg4));                           
        return 1;
    });

	vec2Funcs["move_toward"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).move_toward(arg1, arg2));                           
        return 1;
    });

	vec2Funcs["slide"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).slide(arg1));                           
        return 1;
    });

	vec2Funcs["bounce"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).bounce(arg1));                           
        return 1;
    });

	vec2Funcs["reflect"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).reflect(arg1));                           
        return 1;
    });

	vec2Funcs["is_equal_approx"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).is_equal_approx(arg1));                           
        return 1;
    });

	vec2Funcs["angle"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).angle());                           
        return 1;
    });

	vec2Funcs["from_angle"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).from_angle(arg1));                           
        return 1;
    });

	vec2Funcs["abs"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).abs());                           
        return 1;
    });

	vec2Funcs["rotated"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).rotated(arg1));                           
        return 1;
    });

	vec2Funcs["orthogonal"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).orthogonal());                           
        return 1;
    });

	vec2Funcs["sign"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).sign());                           
        return 1;
    });

	vec2Funcs["floor"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).floor());                           
        return 1;
    });

	vec2Funcs["ceil"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).ceil());                           
        return 1;
    });

	vec2Funcs["round"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).round());                           
        return 1;
    });

	vec2Funcs["snapped"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).snapped(arg1));                           
        return 1;
    });

	vec2Funcs["clamp"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).clamp(arg1, arg2));                           
        return 1;
    });

	vec2Funcs["aspect"] = LUA_LAMBDA_TEMPLATE({
        Variant* var = (Variant*)lua_touserdata(inner_state, lua_upvalueindex(1));
        lua->pushVariant((var->operator Vector2()).aspect());                           
        return 1;
    });


 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__index" , {
        Variant value = arg1.get( arg2 );
        if (!value.is_null()) {
            lua->pushVariant( value ) ;
            return 1;
        }
        
        // Index was not found, so check to see if there is a matching function
        if (vec2Funcs.find(arg2.operator String()) != vec2Funcs.end()) {
            auto f = vec2Funcs.at(arg2.operator String());
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
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__add" , {
        lua->pushVariant( arg1.operator Vector2() + arg2.operator Vector2() );
    return 1;
    });
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__sub" , {
        lua->pushVariant( arg1.operator Vector2() - arg2.operator Vector2() );
    return 1;
    });
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__mul" , {
        switch( arg2.get_type() ){
            case Variant::Type::VECTOR2:
                lua->pushVariant( arg1.operator Vector2() * arg2.operator Vector2() );
                return 1;
            case Variant::Type::INT:
            case Variant::Type::FLOAT:
                lua->pushVariant( arg1.operator Vector2() * arg2.operator double() );
                return 1;
            default:
                return 0;
        }
    });
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__div" , {
        switch( arg2.get_type() ){
            case Variant::Type::VECTOR2:
                lua->pushVariant( arg1.operator Vector2() / arg2.operator Vector2() );
                return 1;
            case Variant::Type::INT:
            case Variant::Type::FLOAT:
                lua->pushVariant( arg1.operator Vector2() / arg2.operator double() );
                return 1;
            default:
                return 0;
        }
    });
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__eq" , {
        lua->pushVariant( arg1.operator Vector2() == arg2.operator Vector2() );
        return 1;
    });

	LUA_METAMETHOD_TEMPLATE( state , -1 , "__lt" , {
        lua->pushVariant( arg1.operator Vector2() < arg2.operator Vector2() );
        return 1;
    });

	LUA_METAMETHOD_TEMPLATE( state , -1 , "__le" , {
        lua->pushVariant( arg1.operator Vector2() <= arg2.operator Vector2() );
        return 1;
    });
 
    lua_pop(state,1); // Stack is now unmodified
}
 
// Create metatable for Vector3 and saves it at LUA_REGISTRYINDEX with name "mt_Vector3"
void Lua::createVector3Metatable( ){
 
    luaL_newmetatable( state , "mt_Vector3" );
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__index" , {
        lua->pushVariant( arg1.get( arg2 ) );
        return 1;
    });
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__newindex" , {
        // We can't use arg1 here because we need to reference the userdata
        ((Variant*)lua_touserdata(inner_state,1))->set( arg2 , arg3 );
        return 0;
    }); 
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__add" , {
        lua->pushVariant( arg1.operator Vector3() + arg2.operator Vector3() );
    return 1;
    });
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__sub" , {
        lua->pushVariant( arg1.operator Vector3() - arg2.operator Vector3() );
    return 1;
    });
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__mul" , {
        switch( arg2.get_type() ){
            case Variant::Type::VECTOR3:
                lua->pushVariant( arg1.operator Vector3() * arg2.operator Vector3() );
                return 1;
            case Variant::Type::INT:
            case Variant::Type::FLOAT:
                lua->pushVariant( arg1.operator Vector3() * arg2.operator double() );
                return 1;
            default:
                return 0;
        }
    });
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__div" , {
        switch( arg2.get_type() ){
            case Variant::Type::VECTOR3:
                lua->pushVariant( arg1.operator Vector3() / arg2.operator Vector3() );
                return 1;
            case Variant::Type::INT:
            case Variant::Type::FLOAT:
                lua->pushVariant( arg1.operator Vector3() / arg2.operator double() );
                return 1;
            default:
                return 0;
        }
    });
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__eq" , {
        lua->pushVariant( arg1.operator Vector3() == arg2.operator Vector3() );
        return 1;
    });
 
    lua_pop(state,1); // Stack is now unmodified
}
 
// Create metatable for Color and saves it at LUA_REGISTRYINDEX with name "mt_Color"
void Lua::createColorMetatable( ){
 
    luaL_newmetatable( state , "mt_Color" );
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__index" , {
        lua->pushVariant( arg1.get( arg2 ) );
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
 
// Create metatable for Color and saves it at LUA_REGISTRYINDEX with name "mt_Color"
void Lua::createRect2Metatable( ){
 
    luaL_newmetatable( state , "mt_Rect2" );
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__index" , {
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
 
// Create metatable for Color and saves it at LUA_REGISTRYINDEX with name "mt_Color"
void Lua::createPlaneMetatable( ){
 
    luaL_newmetatable( state , "mt_Plane" );
 
    LUA_METAMETHOD_TEMPLATE( state , -1 , "__index" , {
        lua->pushVariant( arg1.get( arg2 ) );
        return 1;
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