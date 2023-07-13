#include "luaState.h"

#include <classes/luaAPI.h>
#include <classes/luaCallableExtra.h>
#include <classes/luaObjectMetatable.h>
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
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");
		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			Variant ret = mt->__index(arg1, api, arg2);
			LuaState::pushVariant(inner_state, ret);
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__newindex", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");
		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaError *err = mt->__newindex(arg1, api, arg2, arg3);
			if (err != nullptr) {
				LuaState::pushVariant(inner_state, err);
				return 1;
			}
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__call", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");
		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			int argc = lua_gettop(inner_state);

			Array args;
			for (int i = 1; i < argc; i++) {
				args.push_back(LuaState::getVariant(inner_state, i + 1));
			}

			Variant ret = mt->__call(arg1, api, LuaTuple::fromArray(args));
			LuaState::pushVariant(inner_state, ret);
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__gc", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");
		// Sometimes the api ref is cleaned up first, so we need to check for that
		if (!mt.is_valid() && api.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaError *err = mt->__gc(arg1, api);
			if (err != nullptr) {
				LuaState::pushVariant(inner_state, err);
			}
		}

		// We need to manually uncount the ref
		if (Ref<RefCounted> ref = Object::cast_to<RefCounted>(arg1); ref.is_valid()) {
			ref->~RefCounted();
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__tostring", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");
		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaState::pushVariant(inner_state, mt->__tostring(arg1, api));
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__metatable", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");
		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaState::pushVariant(inner_state, mt->__metatable(arg1, api));
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__len", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");
		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaState::pushVariant(inner_state, mt->__len(arg1, api));
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__unm", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");
		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaState::pushVariant(inner_state, mt->__unm(arg1, api));
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__add", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");
		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaState::pushVariant(inner_state, mt->__add(arg1, api, arg2));
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__sub", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");
		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaState::pushVariant(inner_state, mt->__sub(arg1, api, arg2));
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__mul", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");
		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaState::pushVariant(inner_state, mt->__mul(arg1, api, arg2));
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__div", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");
		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaState::pushVariant(inner_state, mt->__div(arg1, api, arg2));
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__idiv", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");
		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaState::pushVariant(inner_state, mt->__idiv(arg1, api, arg2));
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__mod", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");
		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaState::pushVariant(inner_state, mt->__mod(arg1, api, arg2));
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__pow", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");
		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaState::pushVariant(inner_state, mt->__pow(arg1, api, arg2));
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__concat", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");
		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaState::pushVariant(inner_state, mt->__concat(arg1, api, arg2));
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__band", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");
		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaState::pushVariant(inner_state, mt->__band(arg1, api, arg2));
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__bor", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");
		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaState::pushVariant(inner_state, mt->__bor(arg1, api, arg2));
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__bxor", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");
		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaState::pushVariant(inner_state, mt->__bxor(arg1, api, arg2));
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__bnot", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");
		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaState::pushVariant(inner_state, mt->__bnot(arg1, api));
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__shl", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");
		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaState::pushVariant(inner_state, mt->__shl(arg1, api, arg2));
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__shr", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");

		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaState::pushVariant(inner_state, mt->__shr(arg1, api, arg2));
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__eq", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");

		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaState::pushVariant(inner_state, mt->__eq(arg1, api, arg2));
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__lt", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");

		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaState::pushVariant(inner_state, mt->__lt(arg1, api, arg2));
			return 1;
		}

		return 0;
	});

	LUA_METAMETHOD_TEMPLATE(L, -1, "__le", {
		Ref<LuaAPI> api = getAPI(inner_state);
		Ref<LuaObjectMetatable> mt = arg1.get("lua_metatable");

		if (!mt.is_valid()) {
			mt = api->getObjectMetatable();
		}

		if (mt.is_valid()) {
			LuaState::pushVariant(inner_state, mt->__le(arg1, api, arg2));
			return 1;
		}

		return 0;
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
