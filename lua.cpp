
#include "lua.h"

Lua::Lua(){
	// Createing lua state instance
	state = luaL_newstate();

	lua_register(state, "print", luaPrint);

	// saving this object into registry
	lua_pushstring(state,"__Lua");
	lua_pushlightuserdata( state , this );
	lua_rawset( state , LUA_REGISTRYINDEX );

	// Creating basic types metatables and saving them in registry
	createVector2Metatable();   // "mt_Vector2"
	createVector3Metatable();   // "mt_Vector3"
    createColorMetatable();     // "mt_Color"
    createRect2Metatable();     // "mt_Rect2"
    createPlaneMetatable();     //  "mt_Plane"
    createObjectMetatable();     //  "mt_Object"

	// Exposing basic types constructors
	exposeConstructors();
}

Lua::~Lua(){
    // Destroying lua state instance
    lua_close(state);
}

// Bind C++ functions to GDScript
void Lua::_bind_methods(){
    ClassDB::bind_method(D_METHOD("bind_libs", "Array"),&Lua::bindLibs);
    ClassDB::bind_method(D_METHOD("add_file", "File"), &Lua::addFile);
    ClassDB::bind_method(D_METHOD("add_string", "Code"), &Lua::addString);
    ClassDB::bind_method(D_METHOD("execute"), &Lua::execute);
    ClassDB::bind_method(D_METHOD("push_variant", "var", "Name"),&Lua::pushGlobalVariant);
    ClassDB::bind_method(D_METHOD("pull_variant", "Name"),&Lua::pullVariant);
    ClassDB::bind_method(D_METHOD("expose_function", "Callable", "LuaFunctionName"),&Lua::exposeFunction);
    ClassDB::bind_method(D_METHOD("call_function", "LuaFunctionName", "Args"), &Lua::callFunction );
    ClassDB::bind_method(D_METHOD("set_error_handler", "Callable"), &Lua::setErrorHandler );
    ClassDB::bind_method(D_METHOD("lua_function_exists","LuaFunctionName"), &Lua::luaFunctionExists);
}

void Lua::setErrorHandler(Callable errorHandler) {
    this->errorHandler = errorHandler;
}

void Lua::bindLibs(Array libs) {
    for (int i = 0; i < libs.size(); i++) {
        String lib = ((String)libs.get(i)).to_lower();
        if (lib=="base") {
            luaL_requiref(state, "", luaopen_base, 1);
   	        lua_pop(state, 1);
            // base will override print, so we take it back. User can still override them selfs
            lua_register(state, "print", luaPrint);
        } else if (lib=="table") {
            luaL_requiref(state, LUA_TABLIBNAME, luaopen_table, 1);
            lua_pop(state, 1);
        }else if (lib=="string") {
            luaL_requiref(state, LUA_STRLIBNAME, luaopen_string, 1);
            lua_pop(state, 1);
        }else if (lib=="math") {
            luaL_requiref(state, LUA_MATHLIBNAME, luaopen_math, 1);
            lua_pop(state, 1);
        }else if (lib=="os") {
            luaL_requiref(state, LUA_OSLIBNAME, luaopen_os, 1);
            lua_pop(state, 1);
        }else if (lib=="io") {
            luaL_requiref(state, LUA_IOLIBNAME, luaopen_io, 1);
            lua_pop(state, 1);
        }else if (lib=="coroutine") {
            luaL_requiref(state, LUA_COLIBNAME, luaopen_coroutine, 1);
            lua_pop(state, 1);
        }else if (lib=="debug") {
            luaL_requiref(state, LUA_DBLIBNAME, luaopen_debug, 1);
            lua_pop(state, 1);
        }else if (lib=="package") {
            luaL_requiref(state, LUA_LOADLIBNAME, luaopen_package, 1);
            lua_pop(state, 1);
        }else if (lib=="utf8") {
            luaL_requiref(state, LUA_UTF8LIBNAME, luaopen_utf8, 1);
            lua_pop(state, 1);
        }
    }
}

// Returns lua state
lua_State* Lua::getState() {
    return state;
}

// Get one of the callables exposed to lua via its index.
Callable Lua::getCallable(int index){
    if (index < callables.size()) {
        return callables.get(index);
    }
    return Callable();
}

// The function used when lua calls a exposed function
int Lua::luaExposedFuncCall(lua_State *state) {
    // Get referance of the class
    lua_pushstring(state,"__Lua");
    lua_rawget(state,LUA_REGISTRYINDEX);
    Lua* lua = (Lua*) lua_touserdata(state,-1);
    lua_pop(state,1);

    // Get callabes index
    int callIndex = lua_tointeger(state, lua_upvalueindex(1));
    int argc = lua_gettop(state);
    
    Variant arg1 = lua->getVariant(1);
    Variant arg2 = lua->getVariant(2);
    Variant arg3 = lua->getVariant(3);
    Variant arg4 = lua->getVariant(4);
    Variant arg5 = lua->getVariant(5);

    Callable func = lua->getCallable(callIndex);
    if (!func.is_valid()) {
        print_error( vformat("Error during \"Lua::luaExposedFuncCall\" Callable \"%s\" not vlaid.",func) );
        return 0;
    }
    const Variant* args[5] = {
        &arg1,
        &arg2,
        &arg3,
        &arg4,
        &arg5,
    };
    Variant returned;
    Callable::CallError error;
    func.call(args, argc, returned, error);
    if (error.error != error.CALL_OK) {
        // TODO: Better error handling, maybe this should be passed to errorHandler instead since this could be user triggered as well.
        print_error( vformat("Error during \"Lua::luaExposedFuncCall\" on Callable \"%s\": ",func) );
        return 0;
    }    
    
    lua->pushVariant(returned);
    return 1;

}

// expose a Callable to lua
void Lua::exposeFunction(Callable func, String name){
   callables.push_back(func);

  // Pushing the callables index
  lua_pushinteger(state, callables.size()-1);

  lua_pushcclosure(state, Lua::luaExposedFuncCall, 1);
  // Setting the global name for the function in lua
  lua_setglobal(state, name.ascii().get_data());
  
}

// call a Lua function from GDScript
Variant Lua::callFunction( String function_name, Array args) {
    // put global function name on stack
    lua_getglobal(state, function_name.ascii().get_data() );

    // push args
    for (int i = 0; i < args.size(); ++i) {
        pushVariant(args[i]);
    }

    int ret = lua_pcall(state,args.size(), 1 , 0 );
    if( ret != LUA_OK ){

        if( !errorHandler.is_valid() ){
            print_error( vformat("Error during \"Lua::callFunction\" on Lua function \"%s\": ",function_name) );
        } 
        handleError( ret );
        return 0;
    }
    Variant toReturn = getVariant(1);
    lua_pop(state, 1);
    return toReturn;
}

bool Lua::luaFunctionExists(String function_name){
    int type = lua_getglobal( state , function_name.ascii().get_data() );
    lua_pop(state,1);
    return type == LUA_TFUNCTION;
}

// addFile() calls luaL_loadfille with the absolute file path
void Lua::addFile(String fileName){
    Error error;
    Ref<FileAccess> file = FileAccess::open(fileName, FileAccess::READ, &error);
    if (error != Error::OK) {
        // TODO: Maybe better error handling?
        print_error(error_names[error]);
        return;
    }

    String path = file->get_path_absolute();
    luaL_loadfile(state, path.ascii().get_data());
}

// Run lua string in a thread if threading is enabled
void Lua::addString( String code ){
  luaL_loadstring(state, code.ascii().get_data());
}

// Execute the current lua stack, return error as string if one occures, otherwise return String()
void Lua::execute() {
    // TODO: Maybe some custom error types for better error handling
    int ret = lua_pcall(state, 0, 0, 0);
    if( ret != LUA_OK ){
        if( !errorHandler.is_valid() ){
            print_error( "Error during \"Lua::execute\"" );
        } 
        handleError( ret );
    }
}

// Push a GD Variant to the lua stack and return false if type is not supported (in this case, returns a nil value).
bool Lua::pushVariant(Variant var) {
    switch (var.get_type())
    {
        case Variant::Type::NIL:
            lua_pushnil(state);
            break;
        case Variant::Type::STRING:
            lua_pushstring(state,(var.operator String()).ascii().get_data() );
            break;
        case Variant::Type::INT:
            lua_pushinteger(state, (int64_t)var);
            break;
        case Variant::Type::FLOAT:
            lua_pushnumber(state,var.operator double());
            break;
        case Variant::Type::BOOL:
            lua_pushboolean(state, (bool)var);
            break;
        case Variant::Type::PACKED_BYTE_ARRAY:
        case Variant::Type::PACKED_INT64_ARRAY:
        case Variant::Type::PACKED_INT32_ARRAY:
        case Variant::Type::PACKED_STRING_ARRAY:
        case Variant::Type::PACKED_FLOAT64_ARRAY:
        case Variant::Type::PACKED_FLOAT32_ARRAY:
        case Variant::Type::PACKED_VECTOR2_ARRAY:
        case Variant::Type::PACKED_VECTOR3_ARRAY:
        case Variant::Type::PACKED_COLOR_ARRAY:            
        case Variant::Type::ARRAY: {
            Array array = var.operator Array();
            lua_newtable(state);
            for(int i = 0; i < array.size(); i++) {
                Variant key = i+1;
                Variant value = array[i];
                pushVariant(key);
                pushVariant(value);
                lua_settable(state,-3);
            }
            break;
        }
        case Variant::Type::DICTIONARY:
            lua_newtable(state);
            for(int i = 0; i < ((Dictionary)var).size(); i++) {
                Variant key = ((Dictionary)var).keys()[i];
                Variant value = ((Dictionary)var)[key];
                pushVariant(key);
                pushVariant(value);
                lua_settable(state, -3);
            }
            break;
        case Variant::Type::VECTOR2: {
            void* userdata = (Variant*)lua_newuserdata( state , sizeof(Variant) );
            memcpy( userdata , (void*)&var , sizeof(Variant) );
            luaL_setmetatable(state,"mt_Vector2");
            break;     
        }     
        case Variant::Type::VECTOR3: {
            void* userdata = (Variant*)lua_newuserdata( state , sizeof(Variant) );
            memcpy( userdata , (void*)&var , sizeof(Variant) );
            luaL_setmetatable(state,"mt_Vector3");
            break;     
        }
        case Variant::Type::COLOR: {
            void* userdata = (Variant*)lua_newuserdata( state , sizeof(Variant) );
            memcpy( userdata , (void*)&var , sizeof(Variant) );
            luaL_setmetatable(state,"mt_Color");
            break;
        }
        case Variant::Type::RECT2: {
            void* userdata = (Variant*)lua_newuserdata( state , sizeof(Variant) );
            memcpy( userdata , (void*)&var , sizeof(Variant) );
            luaL_setmetatable(state,"mt_Rect2");
            break;     
        }
        case Variant::Type::PLANE: {
            void* userdata = (Variant*)lua_newuserdata( state , sizeof(Variant) );
            memcpy( userdata , (void*)&var , sizeof(Variant) );
            luaL_setmetatable(state,"mt_Plane");
            break;     
        }
        case Variant::Type::OBJECT: {
            void* userdata = (Variant*)lua_newuserdata( state , sizeof(Variant) );
            memcpy( userdata , (void*)&var , sizeof(Variant) );
            luaL_setmetatable(state,"mt_Object");
            break;  
        }
        default:
            print_error( vformat("Can't pass Variants of type \"%s\" to Lua." , Variant::get_type_name( var.get_type() ) ) );
            lua_pushnil(state);
            return false;
    }
    return true;
}

// Call pushVariant() and set it to a global name
bool Lua::pushGlobalVariant(Variant var, String name) {
    if (pushVariant(var)) {
        lua_setglobal(state,name.ascii().get_data());
        return true;
    }
    return false;
}

// Pull a global variant from Lua to GDScript
Variant Lua::pullVariant(String name){
    Variant val = getVariant(1);
    lua_pop(state, 1);
    return val;
}

// get a value at the given index and return as a variant
Variant Lua::getVariant(int index) {
    Variant result;
    int type = lua_type(state, index);
    switch (type) {
        case LUA_TSTRING:
            result = lua_tostring(state, index);
            break;
        case LUA_TNUMBER:
            result = lua_tonumber(state, index);
            break;
        case LUA_TBOOLEAN:
            result = (bool)lua_toboolean(state, index);
            break;
        case LUA_TUSERDATA:
            result = *(Variant*)lua_touserdata(state,index);
            break;
        case LUA_TTABLE:
        {
            Dictionary dict;
            lua_pushnil(state);  /* first key */
            while (lua_next( state , (index<0)?(index-1):(index)  ) != 0) {
                Variant key = getVariant(-2);
                Variant value = getVariant(-1);
                dict[key] = value;
                lua_pop(state, 1);
            }
            result = dict;
            break;
        }
        default:
            result = Variant();
    }
    return result;
}

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

// Assumes there is a error in the top of the stack. Pops it.
void Lua::handleError(int lua_error){
    String msg;
    switch( lua_error ){
        case LUA_ERRRUN:
            msg += "[LUA_ERRNUN - runtime error ] ";
            break;
        case LUA_ERRMEM:
            msg += "[LUA_ERRMEM - memory allocation error ] ";
            break;
        case LUA_ERRERR:
            msg += "[LUA_ERRERR - error while handling another error ] ";
            break;
        default: break;
    }
    msg += lua_tostring(state,-1);
    lua_pop(state,1);
    if (!errorHandler.is_valid()) {
        print_error(msg);
        return;
    }
    // custom error handling
    const Variant* args[1];
    Variant temp = msg;
    args[0] = &temp;
    Variant returned;
    Callable::CallError error;
    errorHandler.call(args, 1, returned, error);
    if (error.error != error.CALL_OK) {
        print_error( vformat("Error during \"Lua::handleError\" on Errorhandler Callable \"%s\": ",errorHandler) );
    }
}

// Lua functions
// Change lua's print function to print to the Godot console by default
int Lua::luaPrint(lua_State* state)
{
    int args = lua_gettop(state);
    String final_string;
    for ( int n=1; n<=args; ++n) {
		String it_string;
		
		switch( lua_type(state,n) ){
			case LUA_TUSERDATA:{
				Variant var = *(Variant*) lua_touserdata(state,n);
				it_string = var.operator String();
				break;
			}
			default:{
				it_string = lua_tostring(state, n);
				break;
			}
		}

		final_string += it_string;
		if( n < args ) final_string += ", ";
    }

	print_line( final_string );

    return 0;
}
