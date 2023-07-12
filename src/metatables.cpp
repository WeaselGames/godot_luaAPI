#include "luaState.h"

#include <classes/luaAPI.h>
#include <classes/luaCallableExtra.h>
#include <classes/luaTuple.h>

// These 2 macros helps us in constructing general metamethods.
// We can use "lua" as a "Lua" pointer and arg1, arg2, ..., arg5 as Variants objects
// Check examples in createVector2Metatable
#define LUA_LAMBDA_TEMPLATE(_f_)                             \
	[](lua_State *inner_state) -> int {                      \
		Variant arg1 = LuaState::getVariant(inner_state, 1); \
		Variant arg2 = LuaState::getVariant(inner_state, 2); \
		Variant arg3 = LuaState::getVariant(inner_state, 3); \
		Variant arg4 = LuaState::getVariant(inner_state, 4); \
		Variant arg5 = LuaState::getVariant(inner_state, 5); \
		_f_                                                  \
	}

#define LUA_METAMETHOD_TEMPLATE(lua_state, metatable_index, metamethod_name, _f_) \
	lua_pushstring(lua_state, metamethod_name);                                   \
	lua_pushcfunction(lua_state, LUA_LAMBDA_TEMPLATE(_f_));                       \
	lua_settable(lua_state, metatable_index - 2);

// Expose the default constructors
void LuaState::exposeConstructors() {
	lua_pushcfunction(L, LUA_LAMBDA_TEMPLATE({
		int argc = lua_gettop(inner_state);
		if (argc == 0) {
			LuaState::pushVariant(inner_state, Vector2());
		} else {
			LuaState::pushVariant(inner_state, Vector2(arg1.operator double(), arg2.operator double()));
		}
		return 1;
	}));
	lua_setglobal(L, "Vector2");

	lua_pushcfunction(L, LUA_LAMBDA_TEMPLATE({
		int argc = lua_gettop(inner_state);
		if (argc == 0) {
			LuaState::pushVariant(inner_state, Vector3());
		} else {
			LuaState::pushVariant(inner_state, Vector3(arg1.operator double(), arg2.operator double(), arg3.operator double()));
		}
		return 1;
	}));
	lua_setglobal(L, "Vector3");

	lua_pushcfunction(L, LUA_LAMBDA_TEMPLATE({
		int argc = lua_gettop(inner_state);
		if (argc == 3) {
			LuaState::pushVariant(inner_state, Color(arg1.operator double(), arg2.operator double(), arg3.operator double()));
		} else if (argc == 4) {
			LuaState::pushVariant(inner_state, Color(arg1.operator double(), arg2.operator double(), arg3.operator double(), arg4.operator double()));
		} else {
			LuaState::pushVariant(inner_state, Color());
		}
		return 1;
	}));
	lua_setglobal(L, "Color");

	lua_pushcfunction(L, LUA_LAMBDA_TEMPLATE({
		int argc = lua_gettop(inner_state);
		if (argc == 2) {
			LuaState::pushVariant(inner_state, Rect2(arg1.operator Vector2(), arg2.operator Vector2()));
		} else if (argc == 4) {
			LuaState::pushVariant(inner_state, Rect2(arg1.operator double(), arg2.operator double(), arg3.operator double(), arg4.operator double()));
		} else {
			LuaState::pushVariant(inner_state, Rect2());
		}
		return 1;
	}));
	lua_setglobal(L, "Rect2");

	lua_pushcfunction(L, LUA_LAMBDA_TEMPLATE({
		int argc = lua_gettop(inner_state);
		if (argc == 4) {
			LuaState::pushVariant(inner_state, Plane(arg1.operator double(), arg2.operator double(), arg3.operator double(), arg4.operator double()));
		} else if (argc == 3) {
			LuaState::pushVariant(inner_state, Plane(arg1.operator Vector3(), arg2.operator Vector3(), arg3.operator Vector3()));
		} else {
			LuaState::pushVariant(inner_state, Plane(arg1.operator Vector3(), arg1.operator double()));
		}
		return 1;
	}));
	lua_setglobal(L, "Plane");
}

// Create metatable for Vector2 and saves it at LUA_REGISTRYINDEX with name "mt_Vector2"
void LuaState::createVector2Metatable() {
	luaL_newmetatable(L, "mt_Vector2");

	LUA_METAMETHOD_TEMPLATE(L, -1, "__index", {
		if (arg1.has_method(arg2.operator String())) {
			lua_pushlightuserdata(inner_state, lua_touserdata(inner_state, 1));
			LuaState::pushVariant(inner_state, arg2);
			lua_pushcclosure(inner_state, luaUserdataFuncCall, 2);
			return 1;
		}

		LuaState::pushVariant(inner_state, arg1.get(arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__newindex", {
		// We can't use arg1 here because we need to reference the userdata
		((Variant *)lua_touserdata(inner_state, 1))->set(arg2, arg3);
		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__add", {
		LuaState::pushVariant(inner_state, arg1.operator Vector2() + arg2.operator Vector2());
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__sub", {
		LuaState::pushVariant(inner_state, arg1.operator Vector2() - arg2.operator Vector2());
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__mul", {
		switch (arg2.get_type()) {
			case Variant::Type::VECTOR2:
				LuaState::pushVariant(inner_state, arg1.operator Vector2() * arg2.operator Vector2());
				return 1;
			case Variant::Type::INT:
			case Variant::Type::FLOAT:
				LuaState::pushVariant(inner_state, arg1.operator Vector2() * arg2.operator double());
				return 1;
			default:
				return 0;
		}
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__div", {
		switch (arg2.get_type()) {
			case Variant::Type::VECTOR2:
				LuaState::pushVariant(inner_state, arg1.operator Vector2() / arg2.operator Vector2());
				return 1;
			case Variant::Type::INT:
			case Variant::Type::FLOAT:
				LuaState::pushVariant(inner_state, arg1.operator Vector2() / arg2.operator double());
				return 1;
			default:
				return 0;
		}
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__eq", {
		LuaState::pushVariant(inner_state, arg1.operator Vector2() == arg2.operator Vector2());
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__lt", {
		LuaState::pushVariant(inner_state, arg1.operator Vector2() < arg2.operator Vector2());
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__le", {
		LuaState::pushVariant(inner_state, arg1.operator Vector2() <= arg2.operator Vector2());
		return 1;
	});

	lua_pop(L, 1); // Stack is now unmodified
}

// Create metatable for Vector3 and saves it at LUA_REGISTRYINDEX with name "mt_Vector3"
void LuaState::createVector3Metatable() {
	luaL_newmetatable(L, "mt_Vector3");

	LUA_METAMETHOD_TEMPLATE(L, -1, "__index", {
		if (arg1.has_method(arg2.operator String())) {
			lua_pushlightuserdata(inner_state, lua_touserdata(inner_state, 1));
			LuaState::pushVariant(inner_state, arg2);
			lua_pushcclosure(inner_state, luaUserdataFuncCall, 2);
			return 1;
		}

		LuaState::pushVariant(inner_state, arg1.get(arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__newindex", {
		// We can't use arg1 here because we need to reference the userdata
		((Variant *)lua_touserdata(inner_state, 1))->set(arg2, arg3);
		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__add", {
		LuaState::pushVariant(inner_state, arg1.operator Vector3() + arg2.operator Vector3());
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__sub", {
		LuaState::pushVariant(inner_state, arg1.operator Vector3() - arg2.operator Vector3());
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__mul", {
		switch (arg2.get_type()) {
			case Variant::Type::VECTOR3:
				LuaState::pushVariant(inner_state, arg1.operator Vector3() * arg2.operator Vector3());
				return 1;
			case Variant::Type::INT:
			case Variant::Type::FLOAT:
				LuaState::pushVariant(inner_state, arg1.operator Vector3() * arg2.operator double());
				return 1;
			default:
				return 0;
		}
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__div", {
		switch (arg2.get_type()) {
			case Variant::Type::VECTOR3:
				LuaState::pushVariant(inner_state, arg1.operator Vector3() / arg2.operator Vector3());
				return 1;
			case Variant::Type::INT:
			case Variant::Type::FLOAT:
				LuaState::pushVariant(inner_state, arg1.operator Vector3() / arg2.operator double());
				return 1;
			default:
				return 0;
		}
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__eq", {
		LuaState::pushVariant(inner_state, arg1.operator Vector3() == arg2.operator Vector3());
		return 1;
	});

	lua_pop(L, 1); // Stack is now unmodified
}

// Create metatable for Rect2 and saves it at LUA_REGISTRYINDEX with name "mt_Rect2"
void LuaState::createRect2Metatable() {
	luaL_newmetatable(L, "mt_Rect2");

	LUA_METAMETHOD_TEMPLATE(L, -1, "__index", {
		// Index was not found, so check to see if there is a matching function
		if (arg1.has_method(arg2.operator String())) {
			lua_pushlightuserdata(inner_state, lua_touserdata(inner_state, 1));
			LuaState::pushVariant(inner_state, arg2);
			lua_pushcclosure(inner_state, luaUserdataFuncCall, 2);
			return 1;
		}

		LuaState::pushVariant(inner_state, arg1.get(arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__newindex", {
		// We can't use arg1 here because we need to reference the userdata
		((Variant *)lua_touserdata(inner_state, 1))->set(arg2, arg3);
		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__eq", {
		LuaState::pushVariant(inner_state, arg1.operator Rect2() == arg2.operator Rect2());
		return 1;
	});

	lua_pop(L, 1); // Stack is now unmodified
}

// Create metatable for Plane and saves it at LUA_REGISTRYINDEX with name "mt_Plane"
void LuaState::createPlaneMetatable() {
	luaL_newmetatable(L, "mt_Plane");

	LUA_METAMETHOD_TEMPLATE(L, -1, "__index", {
		if (arg1.has_method(arg2.operator String())) {
			lua_pushlightuserdata(inner_state, lua_touserdata(inner_state, 1));
			LuaState::pushVariant(inner_state, arg2);
			lua_pushcclosure(inner_state, luaUserdataFuncCall, 2);
			return 1;
		}

		LuaState::pushVariant(inner_state, arg1.get(arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__newindex", {
		// We can't use arg1 here because we need to reference the userdata
		((Variant *)lua_touserdata(inner_state, 1))->set(arg2, arg3);
		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__eq", {
		LuaState::pushVariant(inner_state, arg1.operator Plane() == arg2.operator Plane());
		return 1;
	});

	lua_pop(L, 1); // Stack is now unmodified
}

// Create metatable for Color and saves it at LUA_REGISTRYINDEX with name "mt_Color"
void LuaState::createColorMetatable() {
	luaL_newmetatable(L, "mt_Color");

	LUA_METAMETHOD_TEMPLATE(L, -1, "__index", {
		if (arg1.has_method(arg2.operator String())) {
			lua_pushlightuserdata(inner_state, lua_touserdata(inner_state, 1));
			LuaState::pushVariant(inner_state, arg2);
			lua_pushcclosure(inner_state, luaUserdataFuncCall, 2);
			return 1;
		}

		LuaState::pushVariant(inner_state, arg1.get(arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__newindex", {
		// We can't use arg1 here because we need to reference the userdata
		((Variant *)lua_touserdata(inner_state, 1))->set(arg2, arg3);
		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__add", {
		LuaState::pushVariant(inner_state, arg1.operator Color() + arg2.operator Color());
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__sub", {
		LuaState::pushVariant(inner_state, arg1.operator Color() - arg2.operator Color());
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__mul", {
		switch (arg2.get_type()) {
			case Variant::Type::COLOR:
				LuaState::pushVariant(inner_state, arg1.operator Color() * arg2.operator Color());
				return 1;
			case Variant::Type::INT:
			case Variant::Type::FLOAT:
				LuaState::pushVariant(inner_state, arg1.operator Color() * arg2.operator double());
				return 1;
			default:
				return 0;
		}
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__div", {
		switch (arg2.get_type()) {
			case Variant::Type::COLOR:
				LuaState::pushVariant(inner_state, arg1.operator Color() / arg2.operator Color());
				return 1;
			case Variant::Type::INT:
			case Variant::Type::FLOAT:
				LuaState::pushVariant(inner_state, arg1.operator Color() / arg2.operator double());
				return 1;
			default:
				return 0;
		}
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__eq", {
		LuaState::pushVariant(inner_state, arg1.operator Color() == arg2.operator Color());
		return 1;
	});

	lua_pop(L, 1); // Stack is now unmodified
}

// Create metatable for Signal and saves it at LUA_REGISTRYINDEX with name "mt_Signal"
void LuaState::createSignalMetatable() {
	luaL_newmetatable(L, "mt_Signal");

	LUA_METAMETHOD_TEMPLATE(L, -1, "__index", {
		if (arg1.has_method(arg2.operator String())) {
			lua_pushlightuserdata(inner_state, lua_touserdata(inner_state, 1));
			LuaState::pushVariant(inner_state, arg2);
			lua_pushcclosure(inner_state, luaUserdataFuncCall, 2);
			return 1;
		}

		LuaState::pushVariant(inner_state, arg1.get(arg2));
		return 1;
	});

	lua_pop(L, 1); // Stack is now unmodified
}

// Create metatable for any Object and saves it at LUA_REGISTRYINDEX with name "mt_Object"
void LuaState::createObjectMetatable() {
	luaL_newmetatable(L, "mt_Object");

	LUA_METAMETHOD_TEMPLATE(L, -1, "__index", {
		Ref<LuaAPI> api = getAPI(inner_state);

		// If object overrides
		if (arg1.has_method("__index")) {
			LuaState::pushVariant(inner_state, arg1.call("__index", Ref<LuaAPI>(api), arg2));
			return 1;
		}

		bool permissive = api->getPermissive();
		Array allowedFields = Array();
		if (arg1.has_method("lua_fields")) {
			allowedFields = arg1.call("lua_fields");
		}

		// In permissive mode, allowedFields beomces a blacklist.
		if (permissive) {
			if (!allowedFields.has(arg2) && arg1.has_method(arg2.operator String())) {
				lua_pushlightuserdata(inner_state, lua_touserdata(inner_state, 1));
				LuaState::pushVariant(inner_state, arg2);
				lua_pushcclosure(inner_state, luaUserdataFuncCall, 2);
				return 1;
			}

			if (!allowedFields.has(arg2)) {
				Variant var = arg1.get(arg2);
				LuaState::pushVariant(inner_state, var);
				return 1;
			}
			return 0;
		}

		// If the functions is allowed and exists
		if (allowedFields.has(arg2) && arg1.has_method(arg2.operator String())) {
			lua_pushlightuserdata(inner_state, lua_touserdata(inner_state, 1));
			LuaState::pushVariant(inner_state, arg2);
			lua_pushcclosure(inner_state, luaUserdataFuncCall, 2);
			return 1;
		}

		// If the field is allowed
		if (allowedFields.has(arg2)) {
			Variant var = arg1.get(arg2);
			LuaState::pushVariant(inner_state, var);
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__newindex", {
		Ref<LuaAPI> api = getAPI(inner_state);

		// If object overrides
		if (arg1.has_method("__newindex")) {
			LuaState::pushVariant(inner_state, arg1.call("__newindex", Ref<LuaAPI>(api), arg2, arg3));
			return 1;
		}

		bool permissive = api->getPermissive();
		Array allowedFields = Array();
		if (arg1.has_method("lua_fields")) {
			allowedFields = arg1.call("lua_fields");
		}

		if (!permissive && allowedFields.has(arg2)) {
			// We can't use arg1 here because we need to reference the userdata
			((Variant *)lua_touserdata(inner_state, 1))->set(arg2, arg3);
		} else if (permissive && !allowedFields.has(arg2)) { // In permissive mode, allowedFields beomces a blacklist.
			((Variant *)lua_touserdata(inner_state, 1))->set(arg2, arg3);
		}
		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__call", {
		if (!arg1.has_method("__call")) {
			return 0;
		}
		int argc = lua_gettop(inner_state);

		Array args;
		for (int i = 1; i < argc; i++) {
			args.push_back(LuaState::getVariant(inner_state, i + 1));
		}

		LuaState::pushVariant(inner_state, arg1.call("__call", Ref<LuaAPI>(getAPI(inner_state)), LuaTuple::fromArray(args)));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__gc", {
		// We need to manually uncount the ref
		if (Ref<RefCounted> ref = Object::cast_to<RefCounted>(arg1); ref.is_valid()) {
			ref->~RefCounted();
		}

		if (!arg1.has_method("__gc")) {
			return 0;
		}

		// just in case they want to raise an error
		Variant ret = arg1.call("__gc", Ref<LuaAPI>(getAPI(inner_state)));
		if (LuaError *err = dynamic_cast<LuaError *>(ret.operator Object *())) {
			LuaState::pushVariant(inner_state, ret);
		}
		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__tostring", {
		// If object overrides
		if (!arg1.has_method("__tostring")) {
			return 0;
		}

		LuaState::pushVariant(inner_state, arg1.call("__tostring", Ref<LuaAPI>(getAPI(inner_state))));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__metatable", {
		// If object overrides
		if (!arg1.has_method("__metatable")) {
			return 0;
		}

		LuaState::pushVariant(inner_state, arg1.call("__metatable", Ref<LuaAPI>(getAPI(inner_state)), arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__len", {
		// If object overrides
		if (!arg1.has_method("__len")) {
			return 0;
		}

		LuaState::pushVariant(inner_state, arg1.call("__len", Ref<LuaAPI>(getAPI(inner_state))));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__unm", {
		// If object overrides
		if (!arg1.has_method("__unm")) {
			return 0;
		}

		LuaState::pushVariant(inner_state, arg1.call("__unm", Ref<LuaAPI>(getAPI(inner_state))));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__add", {
		// If object overrides
		if (!arg1.has_method("__add")) {
			return 0;
		}

		LuaState::pushVariant(inner_state, arg1.call("__add", Ref<LuaAPI>(getAPI(inner_state)), arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__sub", {
		// If object overrides
		if (!arg1.has_method("__sub")) {
			return 0;
		}

		LuaState::pushVariant(inner_state, arg1.call("__sub", Ref<LuaAPI>(getAPI(inner_state)), arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__mul", {
		// If object overrides
		if (!arg1.has_method("__mul")) {
			return 0;
		}

		LuaState::pushVariant(inner_state, arg1.call("__mul", Ref<LuaAPI>(getAPI(inner_state)), arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__div", {
		// If object overrides
		if (!arg1.has_method("__div")) {
			return 0;
		}

		LuaState::pushVariant(inner_state, arg1.call("__div", Ref<LuaAPI>(getAPI(inner_state)), arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__idiv", {
		// If object overrides
		if (!arg1.has_method("__idiv")) {
			return 0;
		}

		LuaState::pushVariant(inner_state, arg1.call("__idiv", Ref<LuaAPI>(getAPI(inner_state)), arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__mod", {
		// If object overrides
		if (!arg1.has_method("__mod")) {
			return 0;
		}

		LuaState::pushVariant(inner_state, arg1.call("__mod", Ref<LuaAPI>(getAPI(inner_state)), arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__pow", {
		// If object overrides
		if (!arg1.has_method("__pow")) {
			return 0;
		}

		LuaState::pushVariant(inner_state, arg1.call("__pow", Ref<LuaAPI>(getAPI(inner_state)), arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__concat", {
		// If object overrides
		if (!arg1.has_method("__concat")) {
			return 0;
		}

		LuaState::pushVariant(inner_state, arg1.call("__concat", Ref<LuaAPI>(getAPI(inner_state)), arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__band", {
		// If object overrides
		if (!arg1.has_method("__band")) {
			return 0;
		}

		LuaState::pushVariant(inner_state, arg1.call("__band", Ref<LuaAPI>(getAPI(inner_state)), arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__bor", {
		// If object overrides
		if (!arg1.has_method("__bor")) {
			return 0;
		}

		LuaState::pushVariant(inner_state, arg1.call("__bor", Ref<LuaAPI>(getAPI(inner_state)), arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__bxor", {
		// If object overrides
		if (!arg1.has_method("__bxor")) {
			return 0;
		}

		LuaState::pushVariant(inner_state, arg1.call("__bxor", Ref<LuaAPI>(getAPI(inner_state)), arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__bnot", {
		// If object overrides
		if (!arg1.has_method("__bnot")) {
			return 0;
		}

		LuaState::pushVariant(inner_state, arg1.call("__bnot", Ref<LuaAPI>(getAPI(inner_state)), arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__shl", {
		// If object overrides
		if (!arg1.has_method("__shl")) {
			return 0;
		}

		LuaState::pushVariant(inner_state, arg1.call("__shl", Ref<LuaAPI>(getAPI(inner_state)), arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__shr", {
		// If object overrides
		if (!arg1.has_method("__shr")) {
			return 0;
		}

		LuaState::pushVariant(inner_state, arg1.call("__shr", Ref<LuaAPI>(getAPI(inner_state)), arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__eq", {
		// If object overrides
		if (!arg1.has_method("__eq")) {
			return 0;
		}

		LuaState::pushVariant(inner_state, arg1.call("__eq", Ref<LuaAPI>(getAPI(inner_state)), arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__lt", {
		// If object overrides
		if (!arg1.has_method("__lt")) {
			return 0;
		}

		LuaState::pushVariant(inner_state, arg1.call("__lt", Ref<LuaAPI>(getAPI(inner_state)), arg2));
		return 1;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__le", {
		// If object overrides
		if (!arg1.has_method("__le")) {
			return 0;
		}

		LuaState::pushVariant(inner_state, arg1.call("__le", Ref<LuaAPI>(getAPI(inner_state)), arg2));
		return 1;
	});

	lua_pop(L, 1);
}

// Create metatable for any Callable and saves it at LUA_REGISTRYINDEX with name "mt_Callable"
void LuaState::createCallableMetatable() {
	luaL_newmetatable(L, "mt_Callable");

	lua_pushstring(L, "__call");
	lua_pushcfunction(L, luaCallableCall);
	lua_settable(L, -3);

	lua_pop(L, 1);
}

// Create metatable for any Callable and saves it at LUA_REGISTRYINDEX with name "mt_Callable"
void LuaState::createCallableExtraMetatable() {
	luaL_newmetatable(L, "mt_CallableExtra");

	LUA_METAMETHOD_TEMPLATE(L, -1, "__gc", {
		// We need to manually uncount the ref
		if (Ref<RefCounted> ref = Object::cast_to<RefCounted>(arg1); ref.is_valid()) {
			ref->~RefCounted();
		}

		return 0;
	});

	lua_pushstring(L, "__call");
	lua_pushcfunction(L, LuaCallableExtra::call);
	lua_settable(L, -3);

	lua_pop(L, 1);
}
