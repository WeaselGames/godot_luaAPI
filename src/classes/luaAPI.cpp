#include "luaAPI.h"
#include "luaCallable.h"

#include <map>

LuaAPI::LuaAPI() {
	// Createing lua state instance
	state = luaL_newstate();

    // push our custom print function so by default it prints to the GDConsole.
	lua_register(state, "print", luaPrint);

	// saving this object into registry
	lua_pushstring(state, "__OBJECT");
	lua_pushlightuserdata(state, this);
	lua_rawset(state, LUA_REGISTRYINDEX);

	// Creating basic types metatables and saving them in registry
	createVector2Metatable();   // "mt_Vector2"
	createVector3Metatable();   // "mt_Vector3"
    createColorMetatable();     // "mt_Color"
    createRect2Metatable();     // "mt_Rect2"
    createPlaneMetatable();     // "mt_Plane"
    createObjectMetatable();    // "mt_Object"
    createCallableMetatable();  // "mt_Callable"

	// Exposing basic types constructors
	exposeConstructors();
}

LuaAPI::~LuaAPI() {
    // Destroying lua state instance
    lua_close(state);
}

// Bind C++ functions to GDScript
void LuaAPI::_bind_methods() {
    ClassDB::bind_method(D_METHOD("bind_libs", "Array"),&LuaAPI::bindLibs);
    ClassDB::bind_method(D_METHOD("do_file", "File"), &LuaAPI::doFile);
    ClassDB::bind_method(D_METHOD("do_string", "Code"), &LuaAPI::doString);
    ClassDB::bind_method(D_METHOD("push_variant", "var", "Name"), &LuaAPI::pushGlobalVariant);
    ClassDB::bind_method(D_METHOD("pull_variant", "Name"), &LuaAPI::pullVariant);
    ClassDB::bind_method(D_METHOD("expose_constructor", "Object", "LuaConstructorName"), &LuaAPI::exposeObjectConstructor);
    ClassDB::bind_method(D_METHOD("call_function", "LuaFunctionName", "Args"), &LuaAPI::callFunction);
    ClassDB::bind_method(D_METHOD("function_exists","LuaFunctionName"), &LuaAPI::luaFunctionExists);
}

// Binds lua librares with the lua state
void LuaAPI::bindLibs(Array libs) {
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
        } else if (lib=="string") {
            luaL_requiref(state, LUA_STRLIBNAME, luaopen_string, 1);
            lua_pop(state, 1);
        } else if (lib=="math") {
            luaL_requiref(state, LUA_MATHLIBNAME, luaopen_math, 1);
            lua_pop(state, 1);
        } else if (lib=="os") {
            luaL_requiref(state, LUA_OSLIBNAME, luaopen_os, 1);
            lua_pop(state, 1);
        } else if (lib=="io") {
            luaL_requiref(state, LUA_IOLIBNAME, luaopen_io, 1);
            lua_pop(state, 1);
        } else if (lib=="coroutine") {
            luaL_requiref(state, LUA_COLIBNAME, luaopen_coroutine, 1);
            lua_pop(state, 1);
        } else if (lib=="debug") {
            luaL_requiref(state, LUA_DBLIBNAME, luaopen_debug, 1);
            lua_pop(state, 1);
        } else if (lib=="package") {
            luaL_requiref(state, LUA_LOADLIBNAME, luaopen_package, 1);
            lua_pop(state, 1);
        } else if (lib=="utf8") {
            luaL_requiref(state, LUA_UTF8LIBNAME, luaopen_utf8, 1);
            lua_pop(state, 1);
        }
    }
}

// call a Lua function from GDScript
Variant LuaAPI::callFunction(String function_name, Array args) {
    // push the error handler on to the stack
    lua_pushcfunction(state, luaErrorHandler);

    // put global function name on stack
    lua_getglobal(state, function_name.ascii().get_data());

    // push args
    for (int i = 0; i < args.size(); ++i) {
        pushVariant(args[i]);
    }

    // error handlers index is -2
    int ret = lua_pcall(state, args.size(), 1, -2);
    if (ret != LUA_OK) {
        return handleError(ret);
    }
    Variant toReturn = getVar(-1); // get return value
    lua_pop(state, 1); // pop err handler
    return toReturn;
}

bool LuaAPI::luaFunctionExists(String function_name) {
    int type = lua_getglobal(state, function_name.ascii().get_data());
    lua_pop(state,1);
    return type == LUA_TFUNCTION;
}

// addFile() calls luaL_loadfille with the absolute file path
LuaError* LuaAPI::doFile(String fileName) {
    // push the error handler onto the stack
    lua_pushcfunction(state, luaErrorHandler);

    Error error;
    Ref<FileAccess> file = FileAccess::open(fileName, FileAccess::READ, &error);
    if (error != Error::OK) {
        return LuaError::newErr(vformat("error '%s' while opening file '%s'", error_names[error], fileName), LuaError::ERR_FILE);
    }

    String path = file->get_path_absolute();
    int ret = luaL_loadfile(state, path.ascii().get_data());
    if (ret != LUA_OK) {
        return handleError(ret);
    }

    LuaError* err = execute(-2);
    // pop the error handler from the stack
    lua_pop(state, 1);
    return err;
}

// Loads string into lua state and executes the top of the stack
LuaError* LuaAPI::doString(String code) {
    // push the error handler onto the stack
    lua_pushcfunction(state, luaErrorHandler);
    luaL_loadstring(state, code.ascii().get_data());
    LuaError* err = execute(-2);
    // pop the error handler from the stack
    lua_pop(state, 1);
    return err;
}

// Execute the current lua stack, return error as string if one occures, otherwise return String()
LuaError* LuaAPI::execute(int handlerIndex) {
    int ret = lua_pcall(state, 0, 0, handlerIndex);
    if (ret != LUA_OK) {
        return handleError(ret);
    }
    return nullptr;
}

// Push a GD Variant to the lua stack and returns a error if the type is not supported
LuaError* LuaAPI::pushVariant(Variant var) const{
    return LuaAPI::pushVariant(var, state);
}

// Push a GD Variant to the lua stack and returns a error if the type is not supported
LuaError* LuaAPI::pushVariant(Variant var, lua_State* state) {
    switch (var.get_type())
    {
        case Variant::Type::NIL:
            lua_pushnil(state);
            break;
        case Variant::Type::STRING:
            lua_pushstring(state,(var.operator String()).ascii().get_data());
            break;
        case Variant::Type::INT:
            lua_pushinteger(state, (int64_t)var);
            break;
        case Variant::Type::FLOAT:
            lua_pushnumber(state, var.operator double());
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
            lua_createtable(state, 0, array.size());

            for(int i = 0; i < array.size(); i++) {
                Variant key = i+1;
                Variant value = array[i];

                LuaError* err = pushVariant(key, state);
                if (err == nullptr)
                    return err;
                
                err = pushVariant(value, state);
                if (err == nullptr)
                    return err;

                lua_settable(state, -3);
            }
            break;
        }
        case Variant::Type::DICTIONARY: {
            Dictionary dict = var.operator Dictionary();
            lua_createtable(state, 0, dict.size());

            for(int i = 0; i < dict.size(); i++) {
                Variant key = dict.keys()[i];
                Variant value = dict[key];
                
                LuaError* err = pushVariant(key, state);
                if (err == nullptr)
                    return err;
                
                err = pushVariant(value, state);
                if (err == nullptr)
                    return err;

                lua_settable(state, -3);
            }
            break;
        }
        case Variant::Type::VECTOR2: {
            void* userdata = (Variant*)lua_newuserdata(state, sizeof(Variant));
            memcpy(userdata, (void*)&var, sizeof(Variant));
            luaL_setmetatable(state, "mt_Vector2");
            break;     
        }     
        case Variant::Type::VECTOR3: {
            void* userdata = (Variant*)lua_newuserdata(state, sizeof(Variant));
            memcpy(userdata, (void*)&var, sizeof(Variant));
            luaL_setmetatable(state, "mt_Vector3");
            break;     
        }
        case Variant::Type::COLOR: {
            void* userdata = (Variant*)lua_newuserdata(state, sizeof(Variant));
            memcpy(userdata, (void*)&var, sizeof(Variant));
            luaL_setmetatable(state, "mt_Color");
            break;
        }
        case Variant::Type::RECT2: {
            void* userdata = (Variant*)lua_newuserdata(state, sizeof(Variant));
            memcpy(userdata, (void*)&var, sizeof(Variant));
            luaL_setmetatable(state, "mt_Rect2");
            break;     
        }
        case Variant::Type::PLANE: {
            void* userdata = (Variant*)lua_newuserdata(state, sizeof(Variant));
            memcpy(userdata, (void*)&var, sizeof(Variant));
            luaL_setmetatable(state, "mt_Plane");
            break;     
        }
        case Variant::Type::OBJECT: {
            // If the type being pushed is a lua error, Raise a error
            if (LuaError* err = Object::cast_to<LuaError>(var.operator Object*()); err != nullptr) {
                lua_pushstring(state, err->getMsg().ascii().get_data());
                lua_error(state);
                break;
            }

            void* userdata = (Variant*)lua_newuserdata(state, sizeof(Variant));
            memcpy(userdata, (void*)&var, sizeof(Variant));
            luaL_setmetatable(state, "mt_Object");
            break;  
        }
        case Variant::Type::CALLABLE: {
            // If the callable type is a luaCallable, just push the actual lua function onto the stack.
            Callable callable = var.operator Callable();
            if (callable.is_custom()) {
                CallableCustom* custom = callable.get_custom();
                LuaCallable* luaCallable = dynamic_cast<LuaCallable*>(custom);
                if (luaCallable != nullptr) {
                    lua_rawgeti(state, LUA_REGISTRYINDEX, luaCallable->getFuncRef());
                    break;
                }
            }

            void* userdata = (Variant*)lua_newuserdata(state, sizeof(Variant));
            memcpy(userdata, (void*)&var, sizeof(Variant));
            luaL_setmetatable(state, "mt_Callable");
            break;  
        }
        default:
            lua_pushnil(state);
            return LuaError::newErr(vformat("can't pass Variants of type \"%s\" to Lua.", Variant::get_type_name(var.get_type())), LuaError::ERR_TYPE);
    }
    return nullptr;
}

// Call pushVariant() and set it to a global name
LuaError* LuaAPI::pushGlobalVariant(Variant var, String name) {
    LuaError* err = pushVariant(var);
    if (err == nullptr) {
        lua_setglobal(state, name.ascii().get_data());
        return err;
    }
    return err;
}

// Pull a global variant from Lua to GDScript
Variant LuaAPI::pullVariant(String name) {
    lua_getglobal(state, name.ascii().get_data());
    Variant val = getVar(1);
    lua_pop(state, 1);
    return val;
}

// get a value at the given index and return as a variant
Variant LuaAPI::getVar(int index) const {
    return getVariant(index, state, Ref<RefCounted>(this));
}

// this cannot be static since we use memnew.
Variant LuaAPI::getVariant(int index, lua_State* state, Ref<RefCounted> obj) {
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
            result = *(Variant*)lua_touserdata(state, index);
            break;
        case LUA_TTABLE:
        {
            lua_len(state, index);
            int len = lua_tointeger(state, -1);
            lua_pop(state, 1);
            // len should be 0 if the type is table and not a array
            if (len) {
                Array array;
                for (int i = 1; i <= len; i++) {
                    lua_geti(state, index, i);
                    array.push_back(getVariant(-1, state, obj));
                    lua_pop(state, 1);
                }
                result = array;
                break;
            }

            lua_pushnil(state);  /* first key */
            Dictionary dict;
            while (lua_next(state, (index<0)?(index-1):(index)) != 0) {
                Variant key = getVariant(-2, state, obj);
                Variant value = getVariant(-1, state, obj);
                dict[key] = value;
                lua_pop(state, 1);
            }
            result = dict;
            break;
        }
        case LUA_TFUNCTION:
        {
            // Put function on the top of the stack and get a ref to it. This will create a copy of the function.
            lua_pushvalue(state, index);
            LuaCallable *callable = memnew(LuaCallable(obj, luaL_ref(state, LUA_REGISTRYINDEX), state));
            result = Callable(callable);
            break;
        }
        case LUA_TNIL:{
            break;
        }
        default:
            result = LuaError::newErr(vformat("unkown lua type '%d' in LuaAPI::getVariant", type), LuaError::ERR_RUNTIME);
    }
    return result;
}



// Creates a new thread staee
lua_State* LuaAPI::newThread() {
    return lua_newthread(state);
}

// returns state
lua_State* LuaAPI::getState() {
    return state;
}

LuaError* LuaAPI::handleError(int lua_error) const {
    return LuaAPI::handleError(lua_error, state);
}

// Assumes there is a error in the top of the stack. Pops it.
LuaError* LuaAPI::handleError(int lua_error, lua_State* state) {
    String msg;
    switch(lua_error) {
        case LUA_ERRRUN: {
            msg += "[LUA_ERRRUN - runtime error ]\n";
            msg += lua_tostring(state, -1);
            msg += "\n";
            lua_pop(state, 1);
            break;
        }
        case LUA_ERRSYNTAX:{
            msg += "[LUA_ERRSYNTAX - syntax error ]\n";
            msg += lua_tostring(state, -1);
            msg += "\n";
            lua_pop(state, 1);
            break;
        }
        case LUA_ERRMEM:{
            msg += "[LUA_ERRMEM - memory allocation error ]\n";
            break;
        }
        case LUA_ERRERR:{
            msg += "[LUA_ERRERR - error while calling LuaAPI::luaErrorHandler ] please report this issue: https://github.com/Trey2k/lua/issues/new\n";
            break;
        }
        case LUA_ERRFILE:{
            msg += "[LUA_ERRFILE - error while opening file]\n";
            break;
        }
        default: break;
    }
    
    return LuaError::newErr(msg, static_cast<LuaError::ErrorType>(lua_error));
}

// for handling callable errors. Does not need the lua object thus is a static method
LuaError* LuaAPI::handleError(const StringName &func, Callable::CallError error, const Variant** p_arguments, int argc) {
    switch (error.error) {
        case Callable::CallError::CALL_ERROR_INVALID_ARGUMENT: {
            return LuaError::newErr(
                vformat("Error calling function: %s - Invalid type for argument %s, expected %s but is %s.", 
                    String(func), 
                    itos(error.argument+1), // lua indexes by 1 so this should be more correct
                    Variant::get_type_name(Variant::Type(error.expected)),
                    Variant::get_type_name(p_arguments[error.argument]->get_type())), 
                LuaError::ERR_RUNTIME);
         }
        case Callable::CallError::CALL_ERROR_TOO_MANY_ARGUMENTS: {
            return LuaError::newErr(
                vformat("Error calling function: %s - Too many arguments, expected %d but got %d.", 
                    String(func), 
                    argc),
                    
                LuaError::ERR_RUNTIME);
        }
        case Callable::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS: {
            return LuaError::newErr(
                vformat("Error calling function: %s - Too few arguments, expected %d but got $d.", 
                    String(func),
                    error.argument,
                    argc), 
                LuaError::ERR_RUNTIME);
        }
        case Callable::CallError::CALL_ERROR_INVALID_METHOD: {
            return LuaError::newErr(
                vformat("Error calling function: %s - Method is invalid.", 
                    String(func)), 
                LuaError::ERR_RUNTIME);
        }
        case Callable::CallError::CALL_ERROR_INSTANCE_IS_NULL: {
            return LuaError::newErr(
                vformat("Error calling function: %s - Instance is null.", 
                    String(func)), 
                LuaError::ERR_RUNTIME);
        }
        default:
            return nullptr;
    }
}

// Lua functions
// Lua error handler, when a error occures it appends the stacktrace to the error message
int LuaAPI::luaErrorHandler(lua_State* state) {
    const char * msg = lua_tostring(state, -1);
    luaL_traceback(state, state, msg, 2);
    lua_remove(state, -2);
    return 1;
}

// Change lua's print function to print to the Godot console by default
int LuaAPI::luaPrint(lua_State* state)
{
    int args = lua_gettop(state);
    String final_string;
    for (int n=1; n<=args; ++n) {
		String it_string;
		
		switch(lua_type(state, n)) {
			case LUA_TUSERDATA:{
				Variant var = *(Variant*) lua_touserdata(state, n);
				it_string = var.operator String();
				break;
			}
			default:{
				it_string = lua_tostring(state, n);
				break;
			}
		}

		final_string += it_string;
		if (n < args) final_string += ", ";
    }

	print_line(final_string);

    return 0;
}

// Used as the __call metamethod for mt_Callable. 
// All exposed gdscript functions are called vis this method.
int LuaAPI::luaCallableCall(lua_State* state) {
    lua_pushstring(state, "__OBJECT");
    lua_rawget(state, LUA_REGISTRYINDEX);
    Ref<RefCounted> OBJ = (Ref<RefCounted>) lua_touserdata(state, -1);
    lua_pop(state, 1);

    int argc = lua_gettop(state)-1; // We subtract 1 becuase the callable its self will be counted
    Callable callable = (Callable) LuaAPI::getVariant(1, state, OBJ);
   
    const Variant **args = (const Variant **)alloca(sizeof(const Variant **) * argc);
    int index = 2; // we start at 2, 1 is the callable
    for (int i = 0; i < argc; i++) {
        Variant* temp = memnew(Variant);
        *temp = LuaAPI::getVariant(index++, state, OBJ);
        if ((*temp).get_type() != Variant::Type::OBJECT) {
            if (LuaError* err = Object::cast_to<LuaError>(temp->operator Object*()); err != nullptr) {
                lua_pushstring(state, err->getMsg().ascii().get_data());
                lua_error(state);
                return 0;
            }
        }

        args[i] = temp;
    }

    Variant returned;
    Callable::CallError error;
    callable.callp(args, argc, returned, error);
    if (error.error != error.CALL_OK) {
        LuaError* err = LuaAPI::handleError(callable.get_method(), error, args, argc);
        lua_pushstring(state, err->getMsg().ascii().get_data());
        lua_error(state);
        return 0;
    }
    
    LuaAPI::pushVariant(returned, state);
    return 1;
}

// This function is invoked whenever a function is called on one of the userdata types 
// excluding mt_Callable or mt_Object if __index is overwritten
int LuaAPI::luaUserdataFuncCall(lua_State* state) {
    lua_pushstring(state, "__OBJECT");
    lua_rawget(state, LUA_REGISTRYINDEX);
    Ref<RefCounted> OBJ = (Ref<RefCounted>) lua_touserdata(state, -1);
    lua_pop(state, 1);

    int argc = lua_gettop(state);

    const Variant **args = (const Variant **)alloca(sizeof(const Variant **) * argc);
    int index = 1;
    for (int i = 0; i < argc; i++) {
        Variant* temp = memnew(Variant);
        *temp = LuaAPI::getVariant(index++, state, OBJ);
        if ((*temp).get_type() != Variant::Type::OBJECT) {
            if (LuaError* err = Object::cast_to<LuaError>(temp->operator Object*()); err != nullptr) {
                lua_pushstring(state, err->getMsg().ascii().get_data());
                lua_error(state);
                return 0;
            }
        }

        args[i] = temp;
    }

    Variant* obj  = (Variant*)lua_touserdata(state, lua_upvalueindex(1));
    String fName = LuaAPI::getVariant(lua_upvalueindex(2), state, OBJ);
    Callable::CallError error;
    Variant ret;
    obj->callp(fName.ascii().get_data(), args, argc, ret, error);
    if (error.error != error.CALL_OK) {
        LuaError* err = LuaAPI::handleError(fName, error, args, argc);
        lua_pushstring(state, err->getMsg().ascii().get_data());
        lua_error(state);
        return 0;
    }

    LuaAPI::pushVariant(ret, state);
    return 1;
}


// -----------meta tables-----------------

// These 2 macros helps us in constructing general metamethods.
// We can use "lua" as a "Lua" pointer and arg1, arg2, ..., arg5 as Variants objects
// Check examples in createVector2Metatable
#define LUA_LAMBDA_TEMPLATE(_f_)                                              \
 [](lua_State* inner_state) -> int {                                          \
     lua_pushstring(inner_state,"__OBJECT");                                  \
     lua_rawget(inner_state,LUA_REGISTRYINDEX);                               \
     Ref<RefCounted> OBJ = (Ref<RefCounted>) lua_touserdata(inner_state, -1); \
     lua_pop(inner_state, 1);                                                 \
     Variant arg1 = LuaAPI::getVariant(1, inner_state, OBJ);                     \
     Variant arg2 = LuaAPI::getVariant(2, inner_state, OBJ);                     \
     Variant arg3 = LuaAPI::getVariant(3, inner_state, OBJ);                     \
     Variant arg4 = LuaAPI::getVariant(4, inner_state, OBJ);                     \
     Variant arg5 = LuaAPI::getVariant(5, inner_state, OBJ);                     \
     _f_                                                                      \
}
 
#define LUA_METAMETHOD_TEMPLATE(lua_state, metatable_index, metamethod_name, _f_)\
lua_pushstring(lua_state, metamethod_name); \
lua_pushcfunction(lua_state, LUA_LAMBDA_TEMPLATE(_f_)); \
lua_settable(lua_state, metatable_index-2);

// Used to keep track of the original pointer via the userdata pointer
static std::map<void*, Variant*> luaObjects;

// Expose the contructor for a object to lua
LuaError* LuaAPI::exposeObjectConstructor(Object* obj, String name) {
    // Make sure we are able to call new
    if (!obj->has_method("new")) {
        return LuaError::newErr("during \"LuaAPI::exposeObjectConstructor\" method 'new' does not exist.", LuaError::ERR_RUNTIME);
    }
    lua_pushlightuserdata(state, obj);
    lua_pushcclosure(state, LUA_LAMBDA_TEMPLATE({
        Object* inner_obj = (Object*)lua_touserdata(inner_state, lua_upvalueindex(1));
        
        // We cant store the variant directly in the userdata. It will causes crashes.
        Variant* var = memnew(Variant);
        *var = inner_obj->call("new");
        void* userdata = (Variant*)lua_newuserdata(inner_state, sizeof(Variant));
        luaObjects[userdata] = var;
        memcpy(userdata, (void*)var, sizeof(Variant));

        luaL_setmetatable(inner_state, "mt_Object");
        return 1;
    }), 1);
    lua_setglobal(state, name.ascii().get_data());
    return nullptr;
}

// Expose the default constructors
void LuaAPI::exposeConstructors() {
    
    lua_pushcfunction(state,LUA_LAMBDA_TEMPLATE({
        int argc = lua_gettop(inner_state);
        if (argc == 0) {
            LuaAPI::pushVariant(Vector2(), inner_state);
        } else {
            LuaAPI::pushVariant(Vector2(arg1.operator double(), arg2.operator double()), inner_state);
        }
        return 1;
    }));
    lua_setglobal(state, "Vector2");
 
    lua_pushcfunction(state,LUA_LAMBDA_TEMPLATE({
        int argc = lua_gettop(inner_state);
        if (argc == 0) {
            LuaAPI::pushVariant(Vector3(), inner_state);
        } else {
            LuaAPI::pushVariant(Vector3(arg1.operator double(), arg2.operator double(), arg3.operator double()), inner_state);
        }
        return 1;
    }));
    lua_setglobal(state, "Vector3");
 
    lua_pushcfunction(state,LUA_LAMBDA_TEMPLATE({
        int argc = lua_gettop(inner_state);
        if (argc == 3) {
            LuaAPI::pushVariant(Color(arg1.operator double(), arg2.operator double(), arg3.operator double()), inner_state);
        } else if (argc == 4) {
            LuaAPI::pushVariant(Color(arg1.operator double(), arg2.operator double(), arg3.operator double(), arg4.operator double()), inner_state);
        } else {
            LuaAPI::pushVariant(Color(), inner_state);
        }
        return 1;
    }));
    lua_setglobal(state, "Color");
 
    lua_pushcfunction(state,LUA_LAMBDA_TEMPLATE({
        int argc = lua_gettop(inner_state);
        if (argc == 2) {
            LuaAPI::pushVariant(Rect2(arg1.operator Vector2(), arg2.operator Vector2()), inner_state);
        } else if (argc == 4) {
            LuaAPI::pushVariant(Rect2(arg1.operator double(), arg2.operator double(), arg3.operator double(), arg4.operator double()), inner_state);
        } else {
            LuaAPI::pushVariant(Rect2(), inner_state);
        }
        return 1;
    }));
    lua_setglobal(state, "Rect2");
 
    lua_pushcfunction(state, LUA_LAMBDA_TEMPLATE({
        int argc = lua_gettop(inner_state);
        if (argc == 4) {
            LuaAPI::pushVariant(Plane(arg1.operator double(), arg2.operator double(), arg3.operator double(), arg4.operator double()), inner_state);
        } else if (argc == 3) {
            LuaAPI::pushVariant(Plane(arg1.operator Vector3(), arg2.operator Vector3(), arg3.operator Vector3()), inner_state);
        } else {
            LuaAPI::pushVariant(Plane(arg1.operator Vector3(), arg1.operator double()), inner_state);
        }
        return 1;
    }));
    lua_setglobal(state, "Plane");
    
}

// Create metatable for Vector2 and saves it at LUA_REGISTRYINDEX with name "mt_Vector2"
void LuaAPI::createVector2Metatable() {
    luaL_newmetatable(state, "mt_Vector2");

    LUA_METAMETHOD_TEMPLATE(state, -1, "__index", {
        if (arg1.has_method(arg2)) {
            lua_pushlightuserdata(inner_state, lua_touserdata(inner_state, 1));
            LuaAPI::pushVariant(arg2, inner_state);
            lua_pushcclosure(inner_state, luaUserdataFuncCall, 2);
            return 1;
        }
        
        LuaAPI::pushVariant(arg1.get(arg2), inner_state);
        return 1;
    });
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__newindex", {
        // We can't use arg1 here because we need to reference the userdata
        ((Variant*)lua_touserdata(inner_state, 1))->set(arg2, arg3);
        return 0;
    }); 
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__add", {
        LuaAPI::pushVariant(arg1.operator Vector2() + arg2.operator Vector2(), inner_state);
    return 1;
    });
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__sub", {
        LuaAPI::pushVariant(arg1.operator Vector2() - arg2.operator Vector2(), inner_state);
    return 1;
    });
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__mul", {
        switch(arg2.get_type()) {
            case Variant::Type::VECTOR2:
                LuaAPI::pushVariant(arg1.operator Vector2() * arg2.operator Vector2(), inner_state);
                return 1;
            case Variant::Type::INT:
            case Variant::Type::FLOAT:
                LuaAPI::pushVariant(arg1.operator Vector2() * arg2.operator double(), inner_state);
                return 1;
            default:
                return 0;
        }
    });
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__div", {
        switch(arg2.get_type()) {
            case Variant::Type::VECTOR2:
                LuaAPI::pushVariant(arg1.operator Vector2() / arg2.operator Vector2(), inner_state);
                return 1;
            case Variant::Type::INT:
            case Variant::Type::FLOAT:
                LuaAPI::pushVariant(arg1.operator Vector2() / arg2.operator double(), inner_state);
                return 1;
            default:
                return 0;
        }
    });
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__eq", {
        LuaAPI::pushVariant(arg1.operator Vector2() == arg2.operator Vector2(), inner_state);
        return 1;
    });

	LUA_METAMETHOD_TEMPLATE(state, -1, "__lt", {
        LuaAPI::pushVariant(arg1.operator Vector2() < arg2.operator Vector2(), inner_state);
        return 1;
    });

	LUA_METAMETHOD_TEMPLATE(state, -1, "__le", {
        LuaAPI::pushVariant(arg1.operator Vector2() <= arg2.operator Vector2(), inner_state);
        return 1;
    });
 
    lua_pop(state, 1); // Stack is now unmodified
}

// Create metatable for Vector3 and saves it at LUA_REGISTRYINDEX with name "mt_Vector3"
void LuaAPI::createVector3Metatable() {
    luaL_newmetatable(state, "mt_Vector3");
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__index", {
        if (arg1.has_method(arg2)) {
            lua_pushlightuserdata(inner_state, lua_touserdata(inner_state,1));
            LuaAPI::pushVariant(arg2, inner_state);
            lua_pushcclosure(inner_state, luaUserdataFuncCall, 2);
            return 1;
        }
        
        LuaAPI::pushVariant(arg1.get(arg2), inner_state);
        return 1;
    });
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__newindex", {
        // We can't use arg1 here because we need to reference the userdata
        ((Variant*)lua_touserdata(inner_state,1))->set(arg2, arg3);
        return 0;
    }); 
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__add", {
        LuaAPI::pushVariant(arg1.operator Vector3() + arg2.operator Vector3(), inner_state);
    return 1;
    });
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__sub", {
        LuaAPI::pushVariant(arg1.operator Vector3() - arg2.operator Vector3(), inner_state);
    return 1;
    });
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__mul", {
        switch(arg2.get_type()) {
            case Variant::Type::VECTOR3:
                LuaAPI::pushVariant(arg1.operator Vector3() * arg2.operator Vector3(), inner_state);
                return 1;
            case Variant::Type::INT:
            case Variant::Type::FLOAT:
                LuaAPI::pushVariant(arg1.operator Vector3() * arg2.operator double(), inner_state);
                return 1;
            default:
                return 0;
        }
    });
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__div", {
        switch(arg2.get_type()) {
            case Variant::Type::VECTOR3:
                LuaAPI::pushVariant(arg1.operator Vector3() / arg2.operator Vector3(), inner_state);
                return 1;
            case Variant::Type::INT:
            case Variant::Type::FLOAT:
                LuaAPI::pushVariant(arg1.operator Vector3() / arg2.operator double(), inner_state);
                return 1;
            default:
                return 0;
        }
    });
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__eq", {
        LuaAPI::pushVariant(arg1.operator Vector3() == arg2.operator Vector3(), inner_state);
        return 1;
    });
 
    lua_pop(state, 1); // Stack is now unmodified
}

// Create metatable for Rect2 and saves it at LUA_REGISTRYINDEX with name "mt_Rect2"
void LuaAPI::createRect2Metatable() {
    luaL_newmetatable(state, "mt_Rect2");
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__index", {
        // Index was not found, so check to see if there is a matching function
        if (arg1.has_method(arg2)) {
            lua_pushlightuserdata(inner_state, lua_touserdata(inner_state,1));
            LuaAPI::pushVariant(arg2, inner_state);
            lua_pushcclosure(inner_state, luaUserdataFuncCall, 2);
            return 1;
        }
        
        LuaAPI::pushVariant(arg1.get(arg2), inner_state);
        return 1;
    });
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__newindex", {
        // We can't use arg1 here because we need to reference the userdata
        ((Variant*)lua_touserdata(inner_state,1))->set(arg2, arg3);
        return 0;
        
    }); 
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__eq", {
        LuaAPI::pushVariant(arg1.operator Rect2() == arg2.operator Rect2(), inner_state);
        return 1;
    });
 
    lua_pop(state, 1); // Stack is now unmodified
}

// Create metatable for Plane and saves it at LUA_REGISTRYINDEX with name "mt_Plane"
void LuaAPI::createPlaneMetatable() {
    luaL_newmetatable(state, "mt_Plane");
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__index", {
        if (arg1.has_method(arg2)) {
            lua_pushlightuserdata(inner_state, lua_touserdata(inner_state,1));
            LuaAPI::pushVariant(arg2, inner_state);
            lua_pushcclosure(inner_state, luaUserdataFuncCall, 2);
            return 1;
        }

        LuaAPI::pushVariant(arg1.get(arg2), inner_state);
        return 1;
    });
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__newindex", {
        // We can't use arg1 here because we need to reference the userdata
        ((Variant*)lua_touserdata(inner_state,1))->set(arg2, arg3);
        return 0;
    }); 
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__eq", {
        LuaAPI::pushVariant(arg1.operator Plane() == arg2.operator Plane(), inner_state);
        return 1;
    });
    
    lua_pop(state, 1); // Stack is now unmodified
}

// Create metatable for Color and saves it at LUA_REGISTRYINDEX with name "mt_Color"
void LuaAPI::createColorMetatable() {
    luaL_newmetatable(state, "mt_Color");
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__index", {
        if (arg1.has_method(arg2)) {
            lua_pushlightuserdata(inner_state, lua_touserdata(inner_state,1));
            LuaAPI::pushVariant(arg2, inner_state);
            lua_pushcclosure(inner_state, luaUserdataFuncCall, 2);
            return 1;
        }
        
        LuaAPI::pushVariant(arg1.get(arg2), inner_state) ;
        return 1;
    });
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__newindex", {
        // We can't use arg1 here because we need to reference the userdata
        ((Variant*)lua_touserdata(inner_state,1))->set(arg2, arg3);
        return 0;
    }); 
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__add", {
        LuaAPI::pushVariant(arg1.operator Color() + arg2.operator Color(), inner_state);
    return 1;
    });
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__sub", {
        LuaAPI::pushVariant(arg1.operator Color() - arg2.operator Color(), inner_state);
    return 1;
    });
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__mul", {
        switch(arg2.get_type()) {
            case Variant::Type::COLOR:
                LuaAPI::pushVariant(arg1.operator Color() * arg2.operator Color(), inner_state);
                return 1;
            case Variant::Type::INT:
            case Variant::Type::FLOAT:
                LuaAPI::pushVariant(arg1.operator Color() * arg2.operator double(), inner_state);
                return 1;
            default:
                return 0;
        }
    });
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__div", {
        switch(arg2.get_type()) {
            case Variant::Type::COLOR:
                LuaAPI::pushVariant(arg1.operator Color() / arg2.operator Color(), inner_state);
                return 1;
            case Variant::Type::INT:
            case Variant::Type::FLOAT:
                LuaAPI::pushVariant(arg1.operator Color() / arg2.operator double(), inner_state);
                return 1;
            default:
                return 0;
        }
    });
 
    LUA_METAMETHOD_TEMPLATE(state, -1, "__eq", {
        LuaAPI::pushVariant(arg1.operator Color() == arg2.operator Color(), inner_state);
        return 1;
    });
 
    lua_pop(state, 1); // Stack is now unmodified
}

// Create metatable for any Object and saves it at LUA_REGISTRYINDEX with name "mt_Object"
void LuaAPI::createObjectMetatable() {
    luaL_newmetatable(state, "mt_Object");

    LUA_METAMETHOD_TEMPLATE(state, -1, "__index", {
        // If object overrides
        if (arg1.has_method("__index")) {
            LuaAPI::pushVariant(arg1.call("__index", arg2), inner_state);
            return 1;
        }

        Array allowedFuncs = Array();
        if (arg1.has_method("lua_funcs")) {
            allowedFuncs = arg1.call("lua_funcs");
        }
        // If the functions is allowed and exists 
        if ((allowedFuncs.is_empty() || allowedFuncs.has(arg2)) && arg1.has_method(arg2)) {
            lua_pushlightuserdata(inner_state, lua_touserdata(inner_state, 1));
            LuaAPI::pushVariant(arg2, inner_state);
            lua_pushcclosure(inner_state, luaUserdataFuncCall, 2);
            return 1;
        }

        Array allowedFields = Array();
        if (arg1.has_method("lua_fields")) {
            allowedFields = arg1.call("lua_fields");
        }
        // If the field is allowed
        if (allowedFields.is_empty() || allowedFields.has(arg2)) {
            Variant var = arg1.get(arg2);
            LuaAPI::pushVariant(var, inner_state);
            return 1;
        }
        
        return 0;
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__newindex", {
        // If object overrides
        if (arg1.has_method("__newindex")) {
            LuaAPI::pushVariant(arg1.call("__newindex", arg2, arg3), inner_state);
            return 1;
        }

        Array allowedFields = Array();
        if (arg1.has_method("lua_fields")) {
            allowedFields = arg1.call("lua_fields");
        }
        
        if (allowedFields.is_empty() || allowedFields.has(arg2)) {
            // We can't use arg1 here because we need to reference the userdata
            ((Variant*)lua_touserdata(inner_state, 1))->set(arg2, arg3);
        }
        return 0;
    }); 
    
    // Makeing sure to clean up the pointer
    LUA_METAMETHOD_TEMPLATE(state, -1, "__gc", {
        void* luaPTR = lua_touserdata(inner_state, 1);
        // Indexing by the userdata pointer to get the og pointer for cleanup
        if (luaObjects.count(luaPTR) > 0) {
            Variant* ptr = luaObjects[luaPTR];
            if (ptr != nullptr) memdelete(ptr);
        }
        return 0;
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__call", {
        if (!arg1.has_method("__call")) {
                return 0;
        }
        int argc = lua_gettop(inner_state);
        
        Array args;
        for (int i = 1; i < argc; i++) {
                args.push_back(LuaAPI::getVariant(i+1, inner_state, OBJ));
        }

        LuaAPI::pushVariant(arg1.call("__call", args), inner_state);
        return 1;
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__tostring", {
        // If object overrides
        if (!arg1.has_method("__tostring")) {
            return 0;
        }

        LuaAPI::pushVariant(arg1.call("__tostring"), inner_state);
        return 1;        
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__metatable", {
        // If object overrides
        if (!arg1.has_method("__metatable")) {
            return 0;
        }
        
        LuaAPI::pushVariant(arg1.call("__metatable", arg2), inner_state);
        return 1;        
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__len", {
        // If object overrides
        if (!arg1.has_method("__len")) {
            return 0;
        }
        
        LuaAPI::pushVariant(arg1.call("__len"), inner_state);
        return 1;        
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__unm", {
        // If object overrides
        if (!arg1.has_method("__unm")) {
            return 0;
        }
        
        LuaAPI::pushVariant(arg1.call("__unm"), inner_state);
        return 1;        
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__add", {
        // If object overrides
        if (!arg1.has_method("__add")) {
            return 0;
        }
        
        LuaAPI::pushVariant(arg1.call("__add", arg2), inner_state);
        return 1;        
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__sub", {
        // If object overrides
        if (!arg1.has_method("__sub")) {
            return 0;
        }
        
        LuaAPI::pushVariant(arg1.call("__sub", arg2), inner_state);
        return 1;        
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__mul", {
        // If object overrides
        if (!arg1.has_method("__mul")) {
            return 0;
        }
        
        LuaAPI::pushVariant(arg1.call("__mul", arg2), inner_state);
        return 1;        
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__div", {
        // If object overrides
        if (!arg1.has_method("__div")) {
            return 0;
        }
        
        LuaAPI::pushVariant(arg1.call("__div", arg2), inner_state);
        return 1;        
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__idiv", {
        // If object overrides
        if (!arg1.has_method("__idiv")) {
            return 0;
        }
        
        LuaAPI::pushVariant(arg1.call("__idiv", arg2), inner_state);
        return 1;        
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__mod", {
        // If object overrides
        if (!arg1.has_method("__mod")) {
            return 0;
        }
        
        LuaAPI::pushVariant(arg1.call("__mod", arg2), inner_state);
        return 1;        
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__pow", {
        // If object overrides
        if (!arg1.has_method("__pow")) {
            return 0;
        }
        
        LuaAPI::pushVariant(arg1.call("__pow", arg2), inner_state);
        return 1;        
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__concat", {
        // If object overrides
        if (!arg1.has_method("__concat")) {
            return 0;
        }
        
        LuaAPI::pushVariant(arg1.call("__concat", arg2), inner_state);
        return 1;        
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__band", {
        // If object overrides
        if (!arg1.has_method("__band")) {
            return 0;
        }
        
        LuaAPI::pushVariant(arg1.call("__band", arg2), inner_state);
        return 1;        
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__bor", {
        // If object overrides
        if (!arg1.has_method("__bor")) {
            return 0;
        }
        
        LuaAPI::pushVariant(arg1.call("__bor", arg2), inner_state);
        return 1;        
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__bxor", {
        // If object overrides
        if (!arg1.has_method("__bxor")) {
            return 0;
        }
        
        LuaAPI::pushVariant(arg1.call("__bxor", arg2), inner_state);
        return 1;        
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__bnot", {
        // If object overrides
        if (!arg1.has_method("__bnot")) {
            return 0;
        }
        
        LuaAPI::pushVariant(arg1.call("__bnot", arg2), inner_state);
        return 1;        
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__shl", {
        // If object overrides
        if (!arg1.has_method("__shl")) {
            return 0;
        }
        
        LuaAPI::pushVariant(arg1.call("__shl", arg2), inner_state);
        return 1;        
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__shr", {
        // If object overrides
        if (!arg1.has_method("__shr")) {
            return 0;
        }
        
        LuaAPI::pushVariant(arg1.call("__shr", arg2), inner_state);
        return 1;        
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__eq", {
        // If object overrides
        if (!arg1.has_method("__eq")) {
            return 0;
        }
        
        LuaAPI::pushVariant(arg1.call("__eq", arg2), inner_state);
        return 1;        
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__lt", {
        // If object overrides
        if (!arg1.has_method("__lt")) {
            return 0;
        }
        
        LuaAPI::pushVariant(arg1.call("__lt", arg2), inner_state);
        return 1;        
    });

    LUA_METAMETHOD_TEMPLATE(state, -1, "__le", {
        // If object overrides
        if (!arg1.has_method("__le")) {
            return 0;
        }
        
        LuaAPI::pushVariant(arg1.call("__le", arg2), inner_state);
        return 1;        
    });

    lua_pop(state, 1);
}

// Create metatable for any Callable and saves it at LUA_REGISTRYINDEX with name "mt_Callable"
void LuaAPI::createCallableMetatable() {
    luaL_newmetatable(state, "mt_Callable");

    lua_pushstring(state, "__call"); 
    lua_pushcfunction(state, luaCallableCall);
    lua_settable(state, -3);
    
    lua_pop(state, 1);
}