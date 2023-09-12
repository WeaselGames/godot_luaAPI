#include "luaState.h"
#include "lua/lua.h"

#include <classes/luaAPI.h>
#include <classes/luaCallableExtra.h>
#include <classes/luaCoroutine.h>
#include <classes/luaTuple.h>

#include <util.h>

void LuaState::setState(lua_State *state, LuaAPI *api, bool bindAPI) {
	this->L = state;
	if (!bindAPI) {
		return;
	}

	// push our custom print function so by default it prints to the GDConsole.
	lua_register(L, "print", luaPrint);

	// saving the object into registry
	lua_pushstring(L, "__LAPI__");
	lua_pushlightuserdata(L, api);
	lua_rawset(L, LUA_REGISTRYINDEX);

	// Creating basic types metatables and saving them in registry
	createVector2Metatable(); // "mt_Vector2"
	createVector3Metatable(); // "mt_Vector3"
	createColorMetatable(); // "mt_Color"
	createRect2Metatable(); // "mt_Rect2"
	createPlaneMetatable(); // "mt_Plane"
	createSignalMetatable(); // "mt_Signal"
	createObjectMetatable(); // "mt_Object"
	createCallableMetatable(); // "mt_Callable"
	createCallableExtraMetatable(); // "mt_CallableExtra"

	// Exposing basic types constructors
	exposeConstructors();
}

lua_State *LuaState::getState() const {
	return L;
}

#ifndef LAPI_LUAJIT

// Binds lua librares with the lua state
void LuaState::bindLibraries(Array libs) {
	for (int i = 0; i < libs.size(); i++) {
		String lib = ((String)libs[i]).to_lower();
		if (lib == "base") {
			luaL_requiref(L, "", luaopen_base, 1);
			lua_pop(L, 1);
			// base will override print, so we take it back. User can still override them self
			lua_register(L, "print", luaPrint);
		} else if (lib == "table") {
			luaL_requiref(L, LUA_TABLIBNAME, luaopen_table, 1);
			lua_pop(L, 1);
		} else if (lib == "string") {
			luaL_requiref(L, LUA_STRLIBNAME, luaopen_string, 1);
			lua_pop(L, 1);
		} else if (lib == "math") {
			luaL_requiref(L, LUA_MATHLIBNAME, luaopen_math, 1);
			lua_pop(L, 1);
		} else if (lib == "os") {
			luaL_requiref(L, LUA_OSLIBNAME, luaopen_os, 1);
			lua_pop(L, 1);
		} else if (lib == "io") {
			luaL_requiref(L, LUA_IOLIBNAME, luaopen_io, 1);
			lua_pop(L, 1);
		} else if (lib == "coroutine") {
			luaL_requiref(L, LUA_COLIBNAME, luaopen_coroutine, 1);
			lua_pop(L, 1);
		} else if (lib == "debug") {
			luaL_requiref(L, LUA_DBLIBNAME, luaopen_debug, 1);
			lua_pop(L, 1);
		} else if (lib == "package") {
			luaL_requiref(L, LUA_LOADLIBNAME, luaopen_package, 1);
			lua_pop(L, 1);
		} else if (lib == "utf8") {
			luaL_requiref(L, LUA_UTF8LIBNAME, luaopen_utf8, 1);
			lua_pop(L, 1);
		}
	}
}

#else

// Binds lua librares with the lua state
void LuaState::bindLibraries(Array libs) {
	for (int i = 0; i < libs.size(); i++) {
		String lib = ((String)libs[i]).to_lower();
		if (lib == "base") {
			lua_pushcfunction(L, luaopen_base);
			lua_pushstring(L, "");
			lua_call(L, 1, 0);

			lua_register(L, "print", luaPrint);
		} else if (lib == "table") {
			lua_pushcfunction(L, luaopen_table);
			lua_pushstring(L, LUA_TABLIBNAME);
			lua_call(L, 1, 0);
		} else if (lib == "string") {
			lua_pushcfunction(L, luaopen_string);
			lua_pushstring(L, LUA_STRLIBNAME);
			lua_call(L, 1, 0);
		} else if (lib == "math") {
			lua_pushcfunction(L, luaopen_math);
			lua_pushstring(L, LUA_MATHLIBNAME);
			lua_call(L, 1, 0);
		} else if (lib == "os") {
			lua_pushcfunction(L, luaopen_os);
			lua_pushstring(L, LUA_OSLIBNAME);
			lua_call(L, 1, 0);
		} else if (lib == "io") {
			lua_pushcfunction(L, luaopen_io);
			lua_pushstring(L, LUA_IOLIBNAME);
			lua_call(L, 1, 0);
		} else if (lib == "debug") {
			lua_pushcfunction(L, luaopen_debug);
			lua_pushstring(L, LUA_DBLIBNAME);
			lua_call(L, 1, 0);
		} else if (lib == "package") {
			lua_pushcfunction(L, luaopen_package);
			lua_pushstring(L, LUA_LOADLIBNAME);
			lua_call(L, 1, 0);
		}
	}
}

#endif

void LuaState::setHook(Callable hook, int mask, int count) {
	if (hook.is_null()) {
		lua_sethook(L, nullptr, 0, 0);
		return;
	}

	lua_pushstring(L, "__HOOK");
	pushVariant(hook);
	lua_settable(L, LUA_REGISTRYINDEX);
	lua_sethook(L, luaHook, mask, count);
}

// Returns true if a lua function exists with the given name
bool LuaState::luaFunctionExists(String functionName) {
	// LuaJIT does not return a type here
	lua_getglobal(L, functionName.ascii().get_data());
	int type = lua_type(L, -1);
	lua_pop(L, 1);
	return type == LUA_TFUNCTION;
}

bool LuaState::luaFunctionExistsRegistry(String functionName) {
	// LuaJIT does not return a type here
	lua_pushvalue(L, LUA_REGISTRYINDEX);
	lua_getfield(L, 1, name.ascii().get_data());
	int type = lua_type(L, -1);
	lua_pop(L, 2);
	return type == LUA_TFUNCTION;
}

// get a value at the given index and return as a variant
Variant LuaState::getVar(int index) const {
	return getVariant(L, index);
}

// Pull a global variant from Lua to GDScript
Variant LuaState::pullVariant(String name) {
	lua_getglobal(L, name.ascii().get_data());
	Variant val = getVar(-1);
	lua_pop(L, 1);
	return val;
}
Variant LuaState::getRegistryKey(String name) {
	lua_pushvalue(L, LUA_REGISTRYINDEX);
	lua_getfield(L, 1, name.ascii().get_data());
	Variant val = getVar(-1);
	lua_pop(L, 2);
	return val;
}

Ref<LuaError> LuaState::setRegistryKey(String name, Variant var) {
	lua_pushvalue(L, LUA_REGISTRYINDEX);
	Ref<LuaError> err = pushVariant(var);
	if (err.is_null()) {
		lua_setfield(L, 2, name.ascii().get_data());
		lua_pop(L, 1);
		return nullptr;
	}
	lua_pop(L, 1);
	return err;
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

Variant LuaState::callFunctionRegistry(String functionName, Array args) {
	// push the error handler on to the stack
	lua_pushcfunction(L, luaErrorHandler);

	// put function name on stack
	lua_pushvalue(L, LUA_REGISTRYINDEX);
	lua_getfield(L, 1, name.ascii().get_data());

	// remove the registry table from the stack
	lua_remove(L, 2);

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
Ref<LuaError> LuaState::pushVariant(Variant var) const {
	return LuaState::pushVariant(L, var);
}

// Call pushVariant() and set it to a global name
Ref<LuaError> LuaState::pushGlobalVariant(String name, Variant var) {
	Ref<LuaError> err = pushVariant(var);
	if (err.is_null()) {
		lua_setglobal(L, name.ascii().get_data());
		return nullptr;
	}
	return err;
}

Ref<LuaError> LuaState::handleError(int lua_error) const {
	return LuaState::handleError(L, lua_error);
}

// --------------
// STATIC METHODS
// --------------

LuaAPI *LuaState::getAPI(lua_State *state) {
	lua_pushstring(state, "__LAPI__");
	lua_rawget(state, LUA_REGISTRYINDEX);
	LuaAPI *api = (LuaAPI *)lua_touserdata(state, -1);
	lua_pop(state, 1);

	return api;
}

// Push a GD Variant to the lua stack and returns a error if the type is not supported
Ref<LuaError> LuaState::pushVariant(lua_State *state, Variant var) {
	switch (var.get_type()) {
		case Variant::Type::NIL:
			lua_pushnil(state);
			break;
		case Variant::Type::STRING:
			lua_pushstring(state, (var.operator String()).ascii().get_data());
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

			for (int i = 0; i < array.size(); i++) {
				Variant key = i + 1;
				Variant value = array[i];

				Ref<LuaError> err = pushVariant(state, key);
				if (!err.is_null()) {
					return err;
				}

				err = pushVariant(state, value);
				if (!err.is_null()) {
					return err;
				}

				lua_settable(state, -3);
			}
			break;
		}
		case Variant::Type::DICTIONARY: {
			Dictionary dict = var.operator Dictionary();
			lua_createtable(state, 0, dict.size());

			Array keys = dict.keys();
			for (int i = 0; i < dict.size(); i++) {
				Variant key = keys[i];
				Variant value = dict[key];

				Ref<LuaError> err = pushVariant(state, key);
				if (!err.is_null()) {
					return err;
				}

				err = pushVariant(state, value);
				if (!err.is_null()) {
					return err;
				}

				lua_settable(state, -3);
			}
			break;
		}
		case Variant::Type::VECTOR2: {
			Variant *userdata = (Variant *)lua_newuserdata(state, sizeof(Variant));
			memnew_placement(userdata, Variant(var));
			luaL_setmetatable(state, "mt_Vector2");
			break;
		}
		case Variant::Type::VECTOR3: {
			Variant *userdata = (Variant *)lua_newuserdata(state, sizeof(Variant));
			memnew_placement(userdata, Variant(var));
			luaL_setmetatable(state, "mt_Vector3");
			break;
		}
		case Variant::Type::COLOR: {
			Variant *userdata = (Variant *)lua_newuserdata(state, sizeof(Variant));
			memnew_placement(userdata, Variant(var));
			luaL_setmetatable(state, "mt_Color");
			break;
		}
		case Variant::Type::RECT2: {
			Variant *userdata = (Variant *)lua_newuserdata(state, sizeof(Variant));
			memnew_placement(userdata, Variant(var));
			luaL_setmetatable(state, "mt_Rect2");
			break;
		}
		case Variant::Type::PLANE: {
			Variant *userdata = (Variant *)lua_newuserdata(state, sizeof(Variant));
			memnew_placement(userdata, Variant(var));
			luaL_setmetatable(state, "mt_Plane");
			break;
		}
		case Variant::Type::SIGNAL: {
			Variant *userdata = (Variant *)lua_newuserdata(state, sizeof(Variant));
			memnew_placement(userdata, Variant(var));
			luaL_setmetatable(state, "mt_Signal");
			break;
		}
		case Variant::Type::OBJECT: {
			if (var.operator Object *() == nullptr) {
				lua_pushnil(state);
				break;
			}

			// If the type being pushed is a lua error, Raise a error
#ifndef LAPI_GDEXTENSION
			if (Ref<LuaError> err = Object::cast_to<LuaError>(var.operator Object *()); !err.is_null()) {
#else
			// blame this on https://github.com/godotengine/godot-cpp/issues/995
			if (Ref<LuaError> err = dynamic_cast<LuaError *>(var.operator Object *()); !err.is_null()) {
#endif
				lua_pushstring(state, err->getMessage().ascii().get_data());
				lua_error(state);
				break;
			}

// If the type being pushed is a tuple, push its content instead.
#ifndef LAPI_GDEXTENSION
			if (Ref<LuaTuple> tuple = Object::cast_to<LuaTuple>(var.operator Object *()); tuple.is_valid()) {
#else
			// blame this on https://github.com/godotengine/godot-cpp/issues/995
			if (Ref<LuaTuple> tuple = dynamic_cast<LuaTuple *>(var.operator Object *()); tuple.is_valid()) {
#endif
				for (int i = 0; i < tuple->size(); i++) {
					Variant value = tuple->get(i);
					pushVariant(state, value);
				}
				break;
			}

// If the type being pushed is a thread, push a LUA_TTHREAD state.
#ifndef LAPI_GDEXTENSION
			if (Ref<LuaCoroutine> thread = Object::cast_to<LuaCoroutine>(var.operator Object *()); thread.is_valid()) {
#else
			// blame this on https://github.com/godotengine/godot-cpp/issues/995
			if (Ref<LuaCoroutine> thread = dynamic_cast<LuaCoroutine *>(var.operator Object *()); thread.is_valid()) {
#endif
				return LuaError::newError("pushing threads is currently not supported.", LuaError::ERR_TYPE);
				break;
			}

			// If the type being pushed is a LuaCallableExtra. use mt_CallableExtra instead
#ifndef LAPI_GDEXTENSION
			if (Ref<LuaCallableExtra> func = Object::cast_to<LuaCallableExtra>(var.operator Object *()); func.is_valid()) {
#else
			// blame this on https://github.com/godotengine/godot-cpp/issues/995
			if (Ref<LuaCallableExtra> func = dynamic_cast<LuaCallableExtra *>(var.operator Object *()); func.is_valid()) {
#endif
				Variant *userdata = (Variant *)lua_newuserdata(state, sizeof(Variant));
				memnew_placement(userdata, Variant(var));
				luaL_setmetatable(state, "mt_CallableExtra");
				break;
			}

			Variant *userdata = (Variant *)lua_newuserdata(state, sizeof(Variant));
			memnew_placement(userdata, Variant(var));
			luaL_setmetatable(state, "mt_Object");
			break;
		}
		case Variant::Type::CALLABLE: {
			Callable callable = var.operator Callable();

			if (!callable.is_valid() || callable.is_null()) {
				lua_pushnil(state);
				break;
			}

			if (callable.is_custom()) {
				// If the type being pushed is a lua function ref, push the ref instead.
#ifndef LAPI_GDEXTENSION
				Ref<LuaAPI> callObj = Object::cast_to<LuaAPI>(callable.get_object());
#else
				Ref<LuaAPI> callObj = dynamic_cast<LuaAPI *>(callable.get_object());
#endif
				if (callObj.is_valid() && (String)callable.get_method() == "call_function_ref") {
					Array argBinds = callable.get_bound_arguments();
					if (argBinds.size() == 1) {
						lua_State *refState = callObj->getState();
						lua_rawgeti(refState, LUA_REGISTRYINDEX, (int)argBinds[0]);
						if (refState != state) {
							lua_xmove(refState, state, 1);
						}
						break;
					}
				}

				// A work around to preserve ref count of CallableCustoms
				Ref<LuaCallableExtra> callableCustom;
				callableCustom.instantiate();
				callableCustom->setInfo(callable, 0, false, false);
				LuaState::pushVariant(state, callableCustom);
				break;
			}

			Variant *userdata = (Variant *)lua_newuserdata(state, sizeof(Variant));
			memnew_placement(userdata, Variant(var));
			luaL_setmetatable(state, "mt_Callable");
			break;
		}
		default:
			lua_pushnil(state);
			return LuaError::newError(vformat("can't pass Variants of type \"%s\" to Lua.", Variant::get_type_name(var.get_type())), LuaError::ERR_TYPE);
	}
	return nullptr;
}

// gets a variant at a given index
Variant LuaState::getVariant(lua_State *state, int index) {
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
			result = *(Variant *)lua_touserdata(state, index);
			break;
		case LUA_TTABLE: {
#ifndef LAPI_LUAJIT
			lua_len(state, index);
#else
			lua_objlen(state, index);
#endif

			int len = lua_tointeger(state, -1);
			lua_pop(state, 1);
			// len should be 0 if the type is table and not a array
			if (len) {
				Array array;
				for (int i = 1; i <= len; i++) {
#ifndef LAPI_LUAJIT
					lua_geti(state, index, i);
#else
					lua_rawgeti(state, index, i);
#endif
					array.push_back(getVariant(state, -1));
					lua_pop(state, 1);
				}
				result = array;
				break;
			}

			lua_pushnil(state); /* first key */
			Dictionary dict;
			while (lua_next(state, (index < 0) ? (index - 1) : (index)) != 0) {
				Variant key = getVariant(state, -2);
				Variant value = getVariant(state, -1);
				dict[key] = value;
				lua_pop(state, 1);
			}
			result = dict;
			break;
		}
		case LUA_TFUNCTION: {
			lua_pushvalue(state, index);
			Array binds;
			binds.push_back(luaL_ref(state, LUA_REGISTRYINDEX));
			result = Callable(getAPI(state), "call_function_ref").bindv(binds);
			break;
		}
		case LUA_TTHREAD: {
			lua_State *tState = lua_tothread(state, index);
			Ref<LuaCoroutine> thread;
			thread.instantiate();
			thread->bindExisting(getAPI(state), tState);
			result = thread;
			break;
		}
		case LUA_TNIL: {
			break;
		}
		default:
			result = LuaError::newError(vformat("Unsupported lua type '%d' in LuaState::getVariant", type), LuaError::ERR_RUNTIME);
	}
	return result;
}

// Assumes there is a error in the top of the stack. Pops it.
Ref<LuaError> LuaState::handleError(lua_State *state, int lua_error) {
	String msg;
	switch (lua_error) {
		case LUA_ERRRUN: {
			msg += "[LUA_ERRRUN - runtime error ]\n";
			msg += lua_tostring(state, -1);
			msg += "\n";
			lua_pop(state, 1);
			break;
		}
		case LUA_ERRSYNTAX: {
			msg += "[LUA_ERRSYNTAX - syntax error ]\n";
			msg += lua_tostring(state, -1);
			msg += "\n";
			lua_pop(state, 1);
			break;
		}
		case LUA_ERRMEM: {
			msg += "[LUA_ERRMEM - memory allocation error ]\n";
			break;
		}
		case LUA_ERRERR: {
			msg += "[LUA_ERRERR - error while calling LuaState::luaErrorHandler ] please report this issue: https://github.com/WeaselGames/lua/issues/new\n";
			break;
		}
		case LUA_ERRFILE: {
			msg += "[LUA_ERRFILE - error while opening file]\n";
			break;
		}
		default:
			break;
	}

	return LuaError::newError(msg, static_cast<LuaError::ErrorType>(lua_error));
}

#ifndef LAPI_GDEXTENSION
// for handling callable errors.
Ref<LuaError> LuaState::handleError(const StringName &func, Callable::CallError error, const Variant **p_arguments, int argc) {
	switch (error.error) {
		case Callable::CallError::CALL_ERROR_INVALID_ARGUMENT: {
			return LuaError::newError(
					vformat("Error calling function: %s - Invalid type for argument %s, expected %s but is %s.",
							String(func),
							itos(error.argument + 1), // lua indexes by 1 so this should be more correct
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

Ref<LuaError> LuaState::handleError(const StringName &func, GDExtensionCallError error, const Variant **p_arguments, int argc) {
	switch (error.error) {
		case GDEXTENSION_CALL_ERROR_INVALID_ARGUMENT: {
			return LuaError::newError(
					vformat("Error calling function: %s - Invalid type for argument %s, expected %s but is %s.",
							String(func),
							itos(error.argument + 1), // lua indexes by 1 so this should be more correct
							Variant::get_type_name(Variant::Type(error.expected)),
							Variant::get_type_name(p_arguments[error.argument]->get_type())),
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
							error.argument,
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

// -------------
// Lua functions
// -------------

// Lua error handler, when a error occurs it appends the stacktrace to the error message
int LuaState::luaErrorHandler(lua_State *state) {
	const char *msg = lua_tostring(state, -1);
	luaL_traceback(state, state, msg, 2);
	lua_remove(state, -2);
	return 1;
}

// Change lua's print function to print to the Godot console by default
int LuaState::luaPrint(lua_State *state) {
	int args = lua_gettop(state);
	String final_string;
	for (int n = 1; n <= args; ++n) {
		String it_string;

		switch (lua_type(state, n)) {
			case LUA_TUSERDATA: {
				Variant var = *(Variant *)lua_touserdata(state, n);
				it_string = var.operator String();
				break;
			}
			case LUA_TBOOLEAN: {
				it_string = lua_toboolean(state, n) ? "true" : "false";
				break;
			}
			default: {
				it_string = lua_tostring(state, n);
				break;
			}
		}

		final_string += it_string;
		if (n < args) {
			final_string += ", ";
		}
	}

	print_line(final_string);

	return 0;
}

#ifndef LAPI_GDEXTENSION

#include <vector>

// Used as the __call metamethod for mt_Callable.
// All exposed gdscript functions are called vis this method.
int LuaState::luaCallableCall(lua_State *state) {
	int argc = lua_gettop(state) - 1; // We subtract 1 because the callable its self will be counted
	Callable callable = (Callable)LuaState::getVariant(state, 1);

	Array args;
	args.resize(argc);
	Vector<const Variant *> mem_args;
	mem_args.resize(argc);

	int index = 2; // we start at 2, 1 is the callable
	for (int i = 0; i < argc; i++) {
		args[i] = LuaState::getVariant(state, index++);
		if (args[i].get_type() != Variant::Type::OBJECT) {
			if (Ref<LuaError> err = Object::cast_to<LuaError>(args[i].operator Object *()); !err.is_null()) {
				lua_pushstring(state, err->getMessage().ascii().get_data());
				lua_error(state);
				return 0;
			}
		}

		mem_args.write[i] = &args[i];
	}

	const Variant **p_args = (const Variant **)mem_args.ptr();

	Variant returned;
	Callable::CallError error;
	callable.callp(p_args, argc, returned, error);
	if (error.error != error.CALL_OK) {
		Ref<LuaError> err = LuaState::handleError(callable.get_method(), error, p_args, argc);
		lua_pushstring(state, err->getMessage().ascii().get_data());
		lua_error(state);
		return 0;
	}

	// await was called, so yield
	if (returned.is_ref_counted() && (returned.operator Object *())->get_class_name() == "GDScriptFunctionState") {
		return lua_yield(state, lua_gettop(state));
	}

	Ref<LuaError> err = LuaState::pushVariant(state, returned);
	if (!err.is_null()) {
		lua_pushstring(state, err->getMessage().ascii().get_data());
		lua_error(state);
		return 0;
	}

	if (returned.get_type() != Variant::Type::OBJECT) {
		return 1;
	}

	if (LuaTuple *tuple = Object::cast_to<LuaTuple>(returned.operator Object *()); tuple != nullptr) {
		return tuple->size();
	}
	return 1;
}

#else

int LuaState::luaCallableCall(lua_State *state) {
	int argc = lua_gettop(state) - 1; // We subtract 1 because the callable its self will be counted
	Callable callable = (Callable)LuaState::getVariant(state, 1);

	Array args;
	int index = 2; // we start at 2, 1 is the callable
	for (int i = 0; i < argc; i++) {
		Variant var = LuaState::getVariant(state, index++);
		if (var.get_type() == Variant::Type::OBJECT) {
			if (Ref<LuaError> err = dynamic_cast<LuaError *>(var.operator Object *()); !err.is_null()) {
				lua_pushstring(state, err->getMessage().ascii().get_data());
				lua_error(state);
				return 0;
			}
		}

		args.append(var);
	}

	Variant returned = callable.callv(args);
	// await was called, so yield
	if (returned.get_type() == Variant::OBJECT && (returned.operator Object *())->get_class() == "GDScriptFunctionState") {
		return lua_yield(state, lua_gettop(state));
	}

	Ref<LuaError> err = LuaState::pushVariant(state, returned);
	if (!err.is_null()) {
		lua_pushstring(state, err->getMessage().ascii().get_data());
		lua_error(state);
		return 0;
	}

	if (returned.get_type() != Variant::Type::OBJECT)
		return 1;
	if (LuaTuple *tuple = dynamic_cast<LuaTuple *>(returned.operator Object *()); tuple != nullptr)
		return tuple->size();
	return 1;
}

#endif

// This function is invoked whenever a function is called on one of the userdata types
// excluding mt_Callable or mt_Object if __index is overwritten
int LuaState::luaUserdataFuncCall(lua_State *state) {
	Variant *obj = (Variant *)lua_touserdata(state, lua_upvalueindex(1));
	String fName = LuaState::getVariant(state, lua_upvalueindex(2));

	int argc = lua_gettop(state);
	Array args;
	args.resize(argc);
	Vector<const Variant *> mem_args;
	mem_args.resize(argc);
	for (int i = 0; i < argc; i++) {
		args[i] = LuaState::getVariant(state, i + 1);
		mem_args.write[i] = &args[i];
	}

	const Variant **p_args = (const Variant **)mem_args.ptr();

	Variant returned;
#ifndef LAPI_GDEXTENSION
	Callable::CallError error;
	obj->callp(fName.ascii().get_data(), p_args, argc, returned, error);
	if (error.error != error.CALL_OK) {
		Ref<LuaError> err = LuaState::handleError(fName, error, p_args, argc);
		lua_pushstring(state, err->getMessage().ascii().get_data());
		lua_error(state);
		return 0;
	}
#else
	GDExtensionCallError error;
	obj->callp(fName.ascii().get_data(), p_args, argc, returned, error);
	if (error.error != GDEXTENSION_CALL_OK) {
		Ref<LuaError> err = LuaState::handleError(fName, error, p_args, argc);
		lua_pushstring(state, err->getMessage().ascii().get_data());
		lua_error(state);
		return 0;
	}
#endif

	LuaState::pushVariant(state, returned);
	if (returned.get_type() != Variant::Type::OBJECT) {
		return 1;
	}

#ifndef LAPI_GDEXTENSION
	if (LuaTuple *tuple = Object::cast_to<LuaTuple>(returned.operator Object *()); tuple != nullptr) {
#else
	// blame this on https://github.com/godotengine/godot-cpp/issues/995
	if (LuaTuple *tuple = dynamic_cast<LuaTuple *>(returned.operator Object *()); tuple != nullptr) {
#endif
		return tuple->size();
	}
	return 1;
}

void LuaState::luaHook(lua_State *state, lua_Debug *ar) {
	lua_pushstring(state, "__HOOK");
	lua_rawget(state, LUA_REGISTRYINDEX);
	Callable hook = LuaState::getVariant(state, -1);
	lua_pop(state, 1);

	if (hook.is_null()) {
		return;
	}

	Array args;
	args.append(Ref<LuaAPI>(getAPI(state)));
	args.append(ar->event);
	args.append(ar->currentline);

#ifndef LAPI_GDEXTENSION
	const int argc = 3;
	const Variant *p_args[argc];
	for (int i = 0; i < argc; i++) {
		p_args[i] = &args[i];
	}

	Variant returned;
	Callable::CallError error;
	hook.callp(p_args, argc, returned, error);
	if (error.error != error.CALL_OK) {
		Ref<LuaError> err = LuaState::handleError(hook.get_method(), error, p_args, argc);
		lua_pushstring(state, err->getMessage().ascii().get_data());
		lua_error(state);
		return;
	}

	if (returned.get_type() == Variant::NIL) {
		return;
	}
#else
	Variant returned = hook.callv(args);
#endif

	Ref<LuaError> err = LuaState::pushVariant(state, returned);
	if (!err.is_null()) {
		lua_pushstring(state, err->getMessage().ascii().get_data());
		lua_error(state);
	}
}
