#include "lua.h"

// These 2 macros helps us in constructing general metamethods.
// We can use "lua" as a "Lua" pointer and arg1, arg2, ..., arg5 as Variants objects
// Check examples in createVector2Metatable
#define LUA_LAMBDA_TEMPLATE(_f_) \
 [](lua_State* inner_state) -> int {                      \
     lua_pushstring(inner_state,"__Lua");                 \
     lua_rawget(inner_state,LUA_REGISTRYINDEX);           \
     Lua* lua = (Lua*) lua_touserdata(inner_state,-1);;   \
     lua_pop(inner_state,1);                              \
     Variant arg1 = lua->getVariant(1);          					\
     Variant arg2 = lua->getVariant(2);          					\
     Variant arg3 = lua->getVariant(3);          					\
     Variant arg4 = lua->getVariant(4);          					\
     Variant arg5 = lua->getVariant(5);          					\
     _f_                                         					\
}
#define LUA_METAMETHOD_TEMPLATE( lua_state , metatable_index , metamethod_name , _f_ )\
lua_pushstring(lua_state,metamethod_name); \
lua_pushcfunction(lua_state,LUA_LAMBDA_TEMPLATE( _f_ )); \
lua_settable(lua_state,metatable_index-2);

void Lua::exposeConstructors( ){
	
	lua_pushcfunction(state,LUA_LAMBDA_TEMPLATE({
        int argc = lua_gettop(inner_state);
        if( argc == 0 ){
			print_line("test six");
            lua->pushVariant( Vector2() );
        } else {
			print_line("test seven", arg1, arg2, arg3, arg4, arg5);
		    lua->pushVariant( Vector2( arg1.operator float() , arg2.operator float() ) );
        }
		return 1;
	}));
	lua_setglobal(state, "Vector2" );

	lua_pushcfunction(state,LUA_LAMBDA_TEMPLATE({
        int argc = lua_gettop(inner_state);
        if( argc == 0 ){
            lua->pushVariant( Vector3() );
        } else {
		    lua->pushVariant( Vector3( arg1.operator float() , arg2.operator float() , arg3.operator float() ) );
        }
        return 1;
	}));
	lua_setglobal(state, "Vector3" );

	lua_pushcfunction(state,LUA_LAMBDA_TEMPLATE({
        int argc = lua_gettop(inner_state);
        if( argc == 3 ){
		    lua->pushVariant( Color( arg1.operator float() , arg2.operator float() , arg3.operator float() ) );
        } else if ( argc == 4 ) {
		    lua->pushVariant( Color( arg1.operator float() , arg2.operator float() , arg3.operator float() , arg4.operator float() ) );
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
		    lua->pushVariant( Rect2( arg1.operator float() , arg2.operator float() , arg3.operator float() , arg4.operator float() ) );
        } else {
            lua->pushVariant( Rect2() );
        }
		return 1;
	}));
	lua_setglobal(state, "Rect2" );

	lua_pushcfunction(state,LUA_LAMBDA_TEMPLATE({
        int argc = lua_gettop(inner_state);
        if( argc == 4 ){
		    lua->pushVariant( Plane( arg1.operator float() , arg2.operator float(), arg3.operator float(), arg4.operator float()) );
        } else if ( argc == 3 ) {
		    lua->pushVariant( Plane( arg1.operator Vector3() , arg2.operator Vector3() , arg3.operator Vector3() ) );
        } else {
            lua->pushVariant( Plane( arg1.operator Vector3() , arg1.operator float()) );
        }
		return 1;
	}));
	lua_setglobal(state, "Plane" );
}

// Create metatable for Vector2 and saves it at LUA_REGISTRYINDEX with name "mt_Vector2"
void Lua::createVector2Metatable( ){
    luaL_newmetatable( state , "mt_Vector2" );

	LUA_METAMETHOD_TEMPLATE( state , -1 , "__index" , {
		print_line("test one", arg1, arg2, arg3, arg4, arg5);
		lua->pushVariant( arg1.get( arg2 ) );
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE( state , -1 , "__newindex" , {
		// We can't use arg1 here because we need to reference the userdata
		print_line("test two");
		((Variant*)lua_touserdata(inner_state,1))->set( arg2 , arg3 );
		return 0;
	});	

	LUA_METAMETHOD_TEMPLATE( state , -1 , "__add" , {
		print_line("test three");
		lua->pushVariant( arg1.operator Vector2() + arg2.operator Vector2() );
    return 1;
	});

	LUA_METAMETHOD_TEMPLATE( state , -1 , "__sub" , {
		print_line("test four");
		lua->pushVariant( arg1.operator Vector2() - arg2.operator Vector2() );
    return 1;
	});

	LUA_METAMETHOD_TEMPLATE( state , -1 , "__mul" , {
		print_line("test five");
		switch( arg2.get_type() ){
			case Variant::Type::VECTOR2:
				lua->pushVariant( arg1.operator Vector2() * arg2.operator Vector2() );
				return 1;
			case Variant::Type::INT:
			case Variant::Type::FLOAT:
				lua->pushVariant( arg1.operator Vector2() * arg2.operator float() );
				return 1;
			default:
				return 0;
		}
	});

	LUA_METAMETHOD_TEMPLATE( state , -1 , "__div" , {
		print_line("test size");
		switch( arg2.get_type() ){
			case Variant::Type::VECTOR2:
				lua->pushVariant( arg1.operator Vector2() / arg2.operator Vector2() );
				return 1;
			case Variant::Type::INT:
			case Variant::Type::FLOAT:
				lua->pushVariant( arg1.operator Vector2() / arg2.operator float() );
				return 1;
			default:
				return 0;
		}
	});

	LUA_METAMETHOD_TEMPLATE( state , -1 , "__eq" , {
		lua->pushVariant( arg1.operator Vector2() == arg2.operator Vector2() );
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
				lua->pushVariant( arg1.operator Vector3() * arg2.operator float() );
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
				lua->pushVariant( arg1.operator Vector3() / arg2.operator float() );
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
				lua->pushVariant( arg1.operator Color() * arg2.operator float() );
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
				lua->pushVariant( arg1.operator Color() / arg2.operator float() );
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