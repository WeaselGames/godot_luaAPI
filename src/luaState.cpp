#include "luaState.h"
#include <classes/luaAPI.h>
#include <classes/luaTuple.h>
#include <classes/luaCallableExtra.h>

#include <util/print_string.h>

#ifndef LAPI_GODOT_EXTENSION
#include <classes/luaCallable.h>
#endif

void LuaState::setState(lua_State *L, RefCounted* obj, bool bindAPI) {
    this->L = L;
    this->obj = obj;
    if (!bindAPI)
        return;

    // push our custom print function so by default it prints to the GDConsole.
    lua_register(L, "print", luaPrint);

    // saving the object into registry
	lua_pushstring(L, "__OBJECT");
	lua_pushlightuserdata(L, obj);
	lua_rawset(L, LUA_REGISTRYINDEX);

    // Creating basic types metatables and saving them in registry
	createVector2Metatable();       // "mt_Vector2"
	createVector3Metatable();       // "mt_Vector3"
    createColorMetatable();         // "mt_Color"
    createRect2Metatable();         // "mt_Rect2"
    createPlaneMetatable();         // "mt_Plane"
    createObjectMetatable();        // "mt_Object"
    createCallableMetatable();      // "mt_Callable"
    createCallableExtraMetatable(); // "mt_CallableExtra"

    // Exposing basic types constructors
	exposeConstructors();
}

lua_State* LuaState::getState() const {
    return L;
}

// Binds lua librares with the lua state
void LuaState::bindLibraries(Array libs) {
    for (int i = 0; i < libs.size(); i++) {
        String lib = ((String)libs[i]).to_lower();
        if (lib=="base") {
            luaL_requiref(L, "", luaopen_base, 1);
   	        lua_pop(L, 1);
            // base will override print, so we take it back. User can still override them selfs
            lua_register(L, "print", luaPrint);
        } else if (lib=="table") {
            luaL_requiref(L, LUA_TABLIBNAME, luaopen_table, 1);
            lua_pop(L, 1);
        } else if (lib=="string") {
            luaL_requiref(L, LUA_STRLIBNAME, luaopen_string, 1);
            lua_pop(L, 1);
        } else if (lib=="math") {
            luaL_requiref(L, LUA_MATHLIBNAME, luaopen_math, 1);
            lua_pop(L, 1);
        } else if (lib=="os") {
            luaL_requiref(L, LUA_OSLIBNAME, luaopen_os, 1);
            lua_pop(L, 1);
        } else if (lib=="io") {
            luaL_requiref(L, LUA_IOLIBNAME, luaopen_io, 1);
            lua_pop(L, 1);
        } else if (lib=="coroutine") {
            luaL_requiref(L, LUA_COLIBNAME, luaopen_coroutine, 1);
            lua_pop(L, 1);
        } else if (lib=="debug") {
            luaL_requiref(L, LUA_DBLIBNAME, luaopen_debug, 1);
            lua_pop(L, 1);
        } else if (lib=="package") {
            luaL_requiref(L, LUA_LOADLIBNAME, luaopen_package, 1);
            lua_pop(L, 1);
        } else if (lib=="utf8") {
            luaL_requiref(L, LUA_UTF8LIBNAME, luaopen_utf8, 1);
            lua_pop(L, 1);
        }
    }
}

// Returns true if a lua function exists with the given name
bool LuaState::luaFunctionExists(String functionName) {
    int type = lua_getglobal(L, functionName.ascii().get_data());
    lua_pop(L, 1);
    return type == LUA_TFUNCTION;
}

// get a value at the given index and return as a variant
Variant LuaState::getVar(int index) const {
    return getVariant(L, index, obj);
}

// Pull a global variant from Lua to GDScript
Variant LuaState::pullVariant(String name) {
    lua_getglobal(L, name.ascii().get_data());
    Variant val = getVar(-1);
    lua_pop(L, 1);
    return val;
}

// call a Lua function from GDScript
Variant LuaState::callFunction(String functionName, Array args) {
    // push the error handler on to the stack
    lua_pushcfunction(L, luaErrorHandler);

    // put global function name on stack
    lua_getglobal(L, functionName.ascii().get_data());

    // push args
    for (int i = 0; i < args.size(); ++i) {
        pushVariant(args[i]);
    }

    // error handlers index is -2 - args.size()
    int ret = lua_pcall(L, args.size(), 1, -2 - args.size());
    if (ret != LUA_OK) {
        return handleError(ret);
    }
    Variant toReturn = getVar(-1); // get return value
    lua_pop(L, 1); // pop err handler
    return toReturn;
}

// Push a GD Variant to the lua stack and returns a error if the type is not supported
LuaError* LuaState::pushVariant(Variant var) const {
    return LuaState::pushVariant(L, var);
}

// Call pushVariant() and set it to a global name
LuaError* LuaState::pushGlobalVariant(String name, Variant var) {
    LuaError* err = pushVariant(var);
    if (err == nullptr) {
        lua_setglobal(L, name.ascii().get_data());
        return err;
    }
    return err;
}

LuaError* LuaState::handleError(int lua_error) const {
    return LuaState::handleError(L, lua_error);
}

// --------------
// STATIC METHODS
// --------------

// Push a GD Variant to the lua stack and returns a error if the type is not supported
LuaError* LuaState::pushVariant(lua_State* state, Variant var) {
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

                LuaError* err = pushVariant(state, key);
                if (err != nullptr)
                    return err;
                
                err = pushVariant(state, value);
                if (err != nullptr)
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
                
                LuaError* err = pushVariant(state, key);
                if (err != nullptr)
                    return err;
                
                err = pushVariant(state, value);
                if (err != nullptr)
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
            #ifndef LAPI_GODOT_EXTENSION
            if (LuaError* err = Object::cast_to<LuaError>(var.operator Object*()); err != nullptr) {
            #else
            // blame this on https://github.com/godotengine/godot-cpp/issues/995
            if (LuaError* err = dynamic_cast<LuaError*>(var.operator Object*()); err != nullptr) {
            #endif
                lua_pushstring(state, err->getMessage().ascii().get_data());
                lua_error(state);
                break;
            }
    
            // If the type being pushed is a tuple, push its content instead.
            #ifndef LAPI_GODOT_EXTENSION
            if (LuaTuple* tuple = Object::cast_to<LuaTuple>(var.operator Object*()); tuple != nullptr) {
            #else
            // blame this on https://github.com/godotengine/godot-cpp/issues/995
            if (LuaTuple* tuple = dynamic_cast<LuaTuple*>(var.operator Object*()); tuple != nullptr) {
            #endif
                for (int i = 0; i < tuple->size(); i++) {
                    Variant value = tuple->get(i);
                    pushVariant(state, value);
                }
                break;
            }

            // If the type being pushed is a LuaCallableExtra. use mt_CallableExtra instead
            #ifndef LAPI_GODOT_EXTENSION
            if (LuaCallableExtra* func = Object::cast_to<LuaCallableExtra>(var.operator Object*()); func != nullptr) {
            #else
            // blame this on https://github.com/godotengine/godot-cpp/issues/995
            if (LuaCallableExtra* func = dynamic_cast<LuaCallableExtra*>(var.operator Object*()); func != nullptr) {
            #endif
                void* userdata = (Variant*)lua_newuserdata(state, sizeof(Variant));
                memcpy(userdata, (void*)&var, sizeof(Variant));
                luaL_setmetatable(state, "mt_CallableExtra");
                break;
            }

            // Temp maybe? If we do not store a referance to refCounted they will die with GDScript
            if (RefCounted* temp = Object::cast_to<RefCounted>(var.operator Object*()); temp != nullptr) {
                lua_pushstring(state, "__OBJECT");
                lua_rawget(state, LUA_REGISTRYINDEX);
                LuaAPI* OBJ = (LuaAPI*) lua_touserdata(state, -1);
                lua_pop(state, 1);

                if (OBJ != nullptr)
                    OBJ->addRef(var);
            }

            void* userdata = (Variant*)lua_newuserdata(state, sizeof(Variant));
            memcpy(userdata, (void*)&var, sizeof(Variant));
            luaL_setmetatable(state, "mt_Object");
            break;  
        }
        case Variant::Type::CALLABLE: {
            #ifndef LAPI_GODOT_EXTENSION
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
            #endif

            void* userdata = (Variant*)lua_newuserdata(state, sizeof(Variant));
            memcpy(userdata, (void*)&var, sizeof(Variant));
            luaL_setmetatable(state, "mt_Callable");
            break;  
        }
        default:
            lua_pushnil(state);
            return LuaError::newError(vformat("can't pass Variants of type \"%s\" to Lua.", Variant::get_type_name(var.get_type())), LuaError::ERR_TYPE);
    }
    return nullptr;
}

// Assumes there is a error in the top of the stack. Pops it.
LuaError* LuaState::handleError(lua_State* state, int lua_error) {
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
            msg += "[LUA_ERRERR - error while calling LuaState::luaErrorHandler ] please report this issue: https://github.com/WeaselGames/lua/issues/new\n";
            break;
        }
        case LUA_ERRFILE:{
            msg += "[LUA_ERRFILE - error while opening file]\n";
            break;
        }
        default: break;
    }
    
    return LuaError::newError(msg, static_cast<LuaError::ErrorType>(lua_error));
}

// for handling callable errors.
#ifndef LAPI_GODOT_EXTENSION
LuaError* LuaState::handleError(const StringName &func, Callable::CallError error, const Variant** p_arguments, int argc) {
    switch (error.error) {
        case Callable::CallError::CALL_ERROR_INVALID_ARGUMENT: {
            return LuaError::newError(
                vformat("Error calling function: %s - Invalid type for argument %s, expected %s but is %s.", 
                    String(func), 
                    itos(error.argument+1), // lua indexes by 1 so this should be more correct
                    Variant::get_type_name(Variant::Type(error.expected)),
                    Variant::get_type_name(p_arguments[error.argument]->get_type())), 
                LuaError::ERR_RUNTIME);
         }
        case Callable::CallError::CALL_ERROR_TOO_MANY_ARGUMENTS: {
            return LuaError::newError(
                vformat("Error calling function: %s - Too many arguments, expected %d but got %d.", 
                    String(func), 
                    argc),
                    
                LuaError::ERR_RUNTIME);
        }
        case Callable::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS: {
            return LuaError::newError(
                vformat("Error calling function: %s - Too few arguments, expected %d but got $d.", 
                    String(func),
                    error.argument,
                    argc), 
                LuaError::ERR_RUNTIME);
        }
        case Callable::CallError::CALL_ERROR_INVALID_METHOD: {
            return LuaError::newError(
                vformat("Error calling function: %s - Method is invalid.", 
                    String(func)), 
                LuaError::ERR_RUNTIME);
        }
        case Callable::CallError::CALL_ERROR_INSTANCE_IS_NULL: {
            return LuaError::newError(
                vformat("Error calling function: %s - Instance is null.", 
                    String(func)), 
                LuaError::ERR_RUNTIME);
        }
        default:
            return nullptr;
    }
}
#else
LuaError* LuaState::handleError(const StringName &func, int error, int expected, int argument, const Variant** p_arguments, int argc) {
    switch (error) {
        case GDEXTENSION_CALL_ERROR_INVALID_ARGUMENT: {
            return LuaError::newError(
                vformat("Error calling function: %s - Invalid type for argument %s, expected %s but is %s.", 
                    String(func), 
                    itos(argument+1), // lua indexes by 1 so this should be more correct
                    Variant::get_type_name(Variant::Type(expected)),
                    Variant::get_type_name(p_arguments[argument]->get_type())), 
                LuaError::ERR_RUNTIME);
         }
        case GDEXTENSION_CALL_ERROR_TOO_MANY_ARGUMENTS: {
            return LuaError::newError(
                vformat("Error calling function: %s - Too many arguments, expected %d but got %d.", 
                    String(func), 
                    argc),
                    
                LuaError::ERR_RUNTIME);
        }
        case GDEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS: {
            return LuaError::newError(
                vformat("Error calling function: %s - Too few arguments, expected %d but got $d.", 
                    String(func),
                    argument,
                    argc), 
                LuaError::ERR_RUNTIME);
        }
        case GDEXTENSION_CALL_ERROR_INVALID_METHOD: {
            return LuaError::newError(
                vformat("Error calling function: %s - Method is invalid.", 
                    String(func)), 
                LuaError::ERR_RUNTIME);
        }
        case GDEXTENSION_CALL_ERROR_INSTANCE_IS_NULL: {
            return LuaError::newError(
                vformat("Error calling function: %s - Instance is null.", 
                    String(func)), 
                LuaError::ERR_RUNTIME);
        }
        default:
            return nullptr;
    }
}
#endif

// gets a variant at a gien index
Variant LuaState::getVariant(lua_State* state, int index, const RefCounted* obj) {
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
        case LUA_TTABLE: {
            lua_len(state, index);
            int len = lua_tointeger(state, -1);
            lua_pop(state, 1);
            // len should be 0 if the type is table and not a array
            if (len) {
                Array array;
                for (int i = 1; i <= len; i++) {
                    lua_geti(state, index, i);
                    array.push_back(getVariant(state, -1, obj));
                    lua_pop(state, 1);
                }
                result = array;
                break;
            }

            lua_pushnil(state);  /* first key */
            Dictionary dict;
            while (lua_next(state, (index<0)?(index-1):(index)) != 0) {
                Variant key = getVariant(state, -2, obj);
                Variant value = getVariant(state, -1, obj);
                dict[key] = value;
                lua_pop(state, 1);
            }
            result = dict;
            break;
        }
        #ifndef LAPI_GODOT_EXTENSION
        case LUA_TFUNCTION: {
            // Put function on the top of the stack and get a ref to it. This will create a copy of the function.
            lua_pushvalue(state, index);
            LuaCallable *callable = memnew(LuaCallable(Ref<RefCounted>(obj), luaL_ref(state, LUA_REGISTRYINDEX), state));
            result = Callable(callable);
            break;
        }
        #endif
        case LUA_TNIL: {
            break;
        }
        default:
            result = LuaError::newError(vformat("unkown lua type '%d' in LuaState::getVariant", type), LuaError::ERR_RUNTIME);
    }
    return result;
}

// -------------
// Lua functions
// -------------

// Lua error handler, when a error occures it appends the stacktrace to the error message
int LuaState::luaErrorHandler(lua_State* state) {
    const char * msg = lua_tostring(state, -1);
    luaL_traceback(state, state, msg, 2);
    lua_remove(state, -2);
    return 1;
}

// Change lua's print function to print to the Godot console by default
int LuaState::luaPrint(lua_State* state)
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
#ifndef LAPI_GODOT_EXTENSION
int LuaState::luaCallableCall(lua_State* state) {
    lua_pushstring(state, "__OBJECT");
    lua_rawget(state, LUA_REGISTRYINDEX);
    RefCounted* OBJ = (RefCounted*) lua_touserdata(state, -1);
    lua_pop(state, 1);

    int argc = lua_gettop(state)-1; // We subtract 1 becuase the callable its self will be counted
    Callable callable = (Callable) LuaState::getVariant(state, 1, OBJ);
   
    const Variant **args = (const Variant **)alloca(sizeof(const Variant **) * argc);
    int index = 2; // we start at 2, 1 is the callable
    for (int i = 0; i < argc; i++) {
        Variant* temp = memnew(Variant);
        *temp = LuaState::getVariant(state, index++, OBJ);
        if ((*temp).get_type() == Variant::Type::OBJECT) {
            if (LuaError* err = Object::cast_to<LuaError>(temp->operator Object*()); err != nullptr) {
                lua_pushstring(state, err->getMessage().ascii().get_data());
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
        LuaError* err = LuaState::handleError(callable.get_method(), error, args, argc);
        lua_pushstring(state, err->getMessage().ascii().get_data());
        lua_error(state);
        return 0;
    }
    
    LuaError* err = LuaState::pushVariant(state, returned);
    if (err != nullptr) {
        lua_pushstring(state, err->getMessage().ascii().get_data());
        lua_error(state);
        return 0;
    }

    if (LuaTuple* tuple = Object::cast_to<LuaTuple>(returned.operator Object*()); tuple != nullptr)
        return tuple->size();
    return 1;
}

#else

int LuaState::luaCallableCall(lua_State* state) {
    lua_pushstring(state, "__OBJECT");
    lua_rawget(state, LUA_REGISTRYINDEX);
    RefCounted* OBJ = (RefCounted*) lua_touserdata(state, -1);
    lua_pop(state, 1);

    int argc = lua_gettop(state)-1; // We subtract 1 becuase the callable its self will be counted
    Callable callable = (Callable) LuaState::getVariant(state, 1, OBJ);
   
    Array args;
    int index = 2; // we start at 2, 1 is the callable
    for (int i = 0; i < argc; i++) {
        
        Variant var = LuaState::getVariant(state, index++, OBJ);
        if (var.get_type() == Variant::Type::OBJECT) {
            if (LuaError* err = dynamic_cast<LuaError*>(var.operator Object*()); err != nullptr) {
                lua_pushstring(state, err->getMessage().ascii().get_data());
                lua_error(state);
                return 0;
            }
        }

        args.append(var);
    }

    Variant returned = callable.callv(args);
    
    LuaError* err = LuaState::pushVariant(state, returned);
    if (err != nullptr) {
        lua_pushstring(state, err->getMessage().ascii().get_data());
        lua_error(state);
        return 0;
    }

    if (LuaTuple* tuple = dynamic_cast<LuaTuple*>(returned.operator Object*()); tuple != nullptr)
        return tuple->size();
    return 1;
}

#endif

// This function is invoked whenever a function is called on one of the userdata types 
// excluding mt_Callable or mt_Object if __index is overwritten
int LuaState::luaUserdataFuncCall(lua_State* state) {
    lua_pushstring(state, "__OBJECT");
    lua_rawget(state, LUA_REGISTRYINDEX);
    RefCounted* OBJ = (RefCounted*) lua_touserdata(state, -1);
    lua_pop(state, 1);

    int argc = lua_gettop(state);

    Array p_args;
    const Variant **args = (const Variant **)alloca(sizeof(const Variant **) * argc);
    int index = 1;
    for (int i = 0; i < argc; i++) {
        p_args.append(LuaState::getVariant(state, index++, OBJ));
        args[i] = &p_args[i];
    }

    Variant* obj  = (Variant*)lua_touserdata(state, lua_upvalueindex(1));
    String fName = LuaState::getVariant(state, lua_upvalueindex(2), OBJ);
    
    Variant returned;
    #ifndef LAPI_GODOT_EXTENSION
    Callable::CallError error;
    obj->callp(fName.ascii().get_data(), args, argc, returned, error);
    if (error.error != error.CALL_OK) {
        LuaError* err = LuaState::handleError(fName, error, args, argc);
        lua_pushstring(state, err->getMessage().ascii().get_data());
        lua_error(state);
        return 0;
    }
    #else
    GDExtensionCallError error;
    obj->callp(fName.ascii().get_data(), args, argc, returned, error);
    if (error.error != GDEXTENSION_CALL_OK) {
        LuaError* err = LuaState::handleError(fName, error.error, error.expected, error.argument, args, argc);
        lua_pushstring(state, err->getMessage().ascii().get_data());
        lua_error(state);
        return 0;
    }
    #endif

    LuaState::pushVariant(state, returned);
    if (LuaTuple* tuple = dynamic_cast<LuaTuple*>(returned.operator Object*()); tuple != nullptr)
        return tuple->size();
    return 1;
}