#include "lua.h"
 
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
 

 
