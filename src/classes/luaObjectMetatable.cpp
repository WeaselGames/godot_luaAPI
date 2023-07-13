#include "luaObjectMetatable.h"

#ifdef LAPI_GDEXTENSION
#define GDVIRTUAL_BIND(m, ...) BIND_VIRTUAL_METHOD(LuaObjectMetatable, m);
#endif

void LuaObjectMetatable::_bind_methods() {
	GDVIRTUAL_BIND(__index, "obj", "lua", "index");
	GDVIRTUAL_BIND(__newindex, "obj", "lua", "index", "value");
	GDVIRTUAL_BIND(__call, "obj", "lua", "args");
	GDVIRTUAL_BIND(__gc, "obj", "lua");
	GDVIRTUAL_BIND(__tostring, "obj", "lua");
	GDVIRTUAL_BIND(__metatable, "obj", "lua");
	GDVIRTUAL_BIND(__len, "obj", "lua");
	GDVIRTUAL_BIND(__unm, "obj", "lua");
	GDVIRTUAL_BIND(__add, "obj", "lua", "other");
	GDVIRTUAL_BIND(__sub, "obj", "lua", "other");
	GDVIRTUAL_BIND(__mul, "obj", "lua", "other");
	GDVIRTUAL_BIND(__div, "obj", "lua", "other");
	GDVIRTUAL_BIND(__idiv, "obj", "lua", "other");
	GDVIRTUAL_BIND(__mod, "obj", "lua", "other");
	GDVIRTUAL_BIND(__pow, "obj", "lua", "other");
	GDVIRTUAL_BIND(__band, "obj", "lua", "other");
	GDVIRTUAL_BIND(__bor, "obj", "lua", "other");
	GDVIRTUAL_BIND(__bxor, "obj", "lua", "other");
	GDVIRTUAL_BIND(__bnot, "obj", "lua");
	GDVIRTUAL_BIND(__shl, "obj", "lua", "other");
	GDVIRTUAL_BIND(__shr, "obj", "lua", "other");
	GDVIRTUAL_BIND(__concat, "obj", "lua", "other");
	GDVIRTUAL_BIND(__eq, "obj", "lua", "other");
	GDVIRTUAL_BIND(__lt, "obj", "lua", "other");
	GDVIRTUAL_BIND(__le, "obj", "lua", "other");
}

Variant LuaObjectMetatable::__index(Object *obj, Ref<LuaAPI> api, String index) {
	Variant ret;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__index, obj, api, index, ret);
#else
	ret = call("__index", obj, api, index);
#endif
	return ret;
}

LuaError *LuaObjectMetatable::__newindex(Object *obj, Ref<LuaAPI> api, String index, Variant value) {
	LuaError *ret = nullptr;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__newindex, obj, api, index, value, ret);
#else
	ret = dynamic_cast<LuaError *>((Object *)call("__newindex", obj, api, index, value));
#endif
	return ret;
}

Variant LuaObjectMetatable::__call(Object *obj, Ref<LuaAPI> api, Ref<LuaTuple> args) {
	Variant ret;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__call, obj, api, args, ret);
#else
	ret = call("__call", obj, api, args);
#endif
	return ret;
}

LuaError *LuaObjectMetatable::__gc(Object *obj, Ref<LuaAPI> api) {
	LuaError *ret = nullptr;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__gc, obj, api, ret);
#else
	ret = dynamic_cast<LuaError *>((Object *)call("__gc", obj, api));
#endif
	return ret;
}

String LuaObjectMetatable::__tostring(Object *obj, Ref<LuaAPI> api) {
	String ret;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__tostring, obj, api, ret);
#else
	ret = call("__tostring", obj, api);
#endif
	return ret;
}

Variant LuaObjectMetatable::__metatable(Object *obj, Ref<LuaAPI> api) {
	Variant ret;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__metatable, obj, api, ret);
#else
	ret = call("__metatable", obj, api);
#endif
	return ret;
}

int LuaObjectMetatable::__len(Object *obj, Ref<LuaAPI> api) {
	int ret = 0;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__len, obj, api, ret);
#else
	ret = call("__len", obj, api);
#endif
	return ret;
}

Variant LuaObjectMetatable::__unm(Object *obj, Ref<LuaAPI> api) {
	Variant ret;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__unm, obj, api, ret);
#else
	ret = call("__unm", obj, api);
#endif
	return ret;
}

Variant LuaObjectMetatable::__add(Object *obj, Ref<LuaAPI> api, Variant other) {
	Variant ret;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__add, obj, api, other, ret);
#else
	ret = call("__add", obj, api, other);
#endif
	return ret;
}

Variant LuaObjectMetatable::__sub(Object *obj, Ref<LuaAPI> api, Variant other) {
	Variant ret;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__sub, obj, api, other, ret);
#else
	ret = call("__sub", obj, api, other);
#endif
	return ret;
}

Variant LuaObjectMetatable::__mul(Object *obj, Ref<LuaAPI> api, Variant other) {
	Variant ret;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__mul, obj, api, other, ret);
#else
	ret = call("__mul", obj, api, other);
#endif
	return ret;
}

Variant LuaObjectMetatable::__div(Object *obj, Ref<LuaAPI> api, Variant other) {
	Variant ret;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__div, obj, api, other, ret);
#else
	ret = call("__div", obj, api, other);
#endif
	return ret;
}

Variant LuaObjectMetatable::__idiv(Object *obj, Ref<LuaAPI> api, Variant other) {
	Variant ret;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__idiv, obj, api, other, ret);
#else
	ret = call("__idiv", obj, api, other);
#endif
	return ret;
}

Variant LuaObjectMetatable::__mod(Object *obj, Ref<LuaAPI> api, Variant other) {
	Variant ret;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__mod, obj, api, other, ret);
#else
	ret = call("__mod", obj, api, other);
#endif
	return ret;
}

Variant LuaObjectMetatable::__pow(Object *obj, Ref<LuaAPI> api, Variant other) {
	Variant ret;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__pow, obj, api, other, ret);
#else
	ret = call("__pow", obj, api, other);
#endif
	return ret;
}

Variant LuaObjectMetatable::__band(Object *obj, Ref<LuaAPI> api, Variant other) {
	Variant ret;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__band, obj, api, other, ret);
#else
	ret = call("__band", obj, api, other);
#endif
	return ret;
}

Variant LuaObjectMetatable::__bor(Object *obj, Ref<LuaAPI> api, Variant other) {
	Variant ret;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__bor, obj, api, other, ret);
#else
	ret = call("__bor", obj, api, other);
#endif
	return ret;
}

Variant LuaObjectMetatable::__bxor(Object *obj, Ref<LuaAPI> api, Variant other) {
	Variant ret;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__bxor, obj, api, other, ret);
#else
	ret = call("__bxor", obj, api, other);
#endif
	return ret;
}

Variant LuaObjectMetatable::__bnot(Object *obj, Ref<LuaAPI> api) {
	Variant ret;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__bnot, obj, api, ret);
#else
	ret = call("__bnot", obj, api);
#endif
	return ret;
}

Variant LuaObjectMetatable::__shl(Object *obj, Ref<LuaAPI> api, Variant other) {
	Variant ret;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__shl, obj, api, other, ret);
#else
	ret = call("__shl", obj, api, other);
#endif
	return ret;
}

Variant LuaObjectMetatable::__shr(Object *obj, Ref<LuaAPI> api, Variant other) {
	Variant ret;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__shr, obj, api, other, ret);
#else
	ret = call("__shr", obj, api, other);
#endif
	return ret;
}

Variant LuaObjectMetatable::__concat(Object *obj, Ref<LuaAPI> api, Variant other) {
	Variant ret;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__concat, obj, api, other, ret);
#else
	ret = call("__concat", obj, api, other);
#endif
	return ret;
}

bool LuaObjectMetatable::__eq(Object *obj, Ref<LuaAPI> api, Variant other) {
	bool ret = false;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__eq, obj, api, other, ret);
#else
	ret = call("__eq", obj, api, other);
#endif
	return ret;
}

bool LuaObjectMetatable::__lt(Object *obj, Ref<LuaAPI> api, Variant other) {
	bool ret = false;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__lt, obj, api, other, ret);
#else
	ret = call("__lt", obj, api, other);
#endif
	return ret;
}

bool LuaObjectMetatable::__le(Object *obj, Ref<LuaAPI> api, Variant other) {
	bool ret = false;
#ifndef LAPI_GDEXTENSION
	GDVIRTUAL_CALL(__le, obj, api, other, ret);
#else
	ret = call("__le", obj, api, other);
#endif
	return ret;
}

// Default object metatable

void LuaDefaultObjectMetatable::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_permissive"), &LuaDefaultObjectMetatable::getPermissive);
	ClassDB::bind_method(D_METHOD("set_permissive", "value"), &LuaDefaultObjectMetatable::setPermissive);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "permissive"), "set_permissive", "get_permissive");
}

void LuaDefaultObjectMetatable::setPermissive(bool value) {
	permissive = value;
}

bool LuaDefaultObjectMetatable::getPermissive() const {
	return permissive;
}

Variant LuaDefaultObjectMetatable::__index(Object *obj, Ref<LuaAPI> api, String index) {
	if (obj->has_method("__index")) {
		return obj->call("__index", api, index);
	}

	Array fields = Array();
	if (obj->has_method("lua_fields")) {
		fields = obj->call("lua_fields");
	}

	if ((!permissive && fields.has(index)) || (permissive && !fields.has(index))) {
		return obj->get(index);
	}

	return Variant();
}

LuaError *LuaDefaultObjectMetatable::__newindex(Object *obj, Ref<LuaAPI> api, String index, Variant value) {
	if (obj->has_method("__newindex")) {
		Variant ret = obj->call("__newindex", api, index, value);
		if (ret.get_type() == Variant::OBJECT) {
#ifndef LAPI_GDEXTENSION
			return Object::cast_to<LuaError>(ret.operator Object *());
#else
			return dynamic_cast<LuaError *>(ret.operator Object *());
#endif
		}
	}

	Array fields = Array();
	if (obj->has_method("lua_fields")) {
		fields = obj->call("lua_fields");
	}

	if ((!permissive && fields.has(index)) || (permissive && !fields.has(index))) {
		obj->set(index, value);
		return nullptr;
	}

	return LuaError::newError(vformat("Attempt to set field '%s' on object of type '%s' which is not a valid field.", index, obj->get_class()), LuaError::ERR_RUNTIME);
}

Variant LuaDefaultObjectMetatable::__call(Object *obj, Ref<LuaAPI> api, Ref<LuaTuple> args) {
	if (obj->has_method("__call")) {
		return obj->call("__call", api, args);
	}

	return Variant();
}

LuaError *LuaDefaultObjectMetatable::__gc(Object *obj, Ref<LuaAPI> api) {
	if (obj->has_method("__gc")) {
		Variant ret = obj->call("__gc", api);
		if (ret.get_type() == Variant::OBJECT) {
#ifndef LAPI_GDEXTENSION
			return Object::cast_to<LuaError>(ret.operator Object *());
#else
			return dynamic_cast<LuaError *>(ret.operator Object *());
#endif
		}
	}

	return nullptr;
}

String LuaDefaultObjectMetatable::__tostring(Object *obj, Ref<LuaAPI> api) {
	if (obj->has_method("__tostring")) {
		return obj->call("__tostring", api);
	}

	return String();
}

Variant LuaDefaultObjectMetatable::__metatable(Object *obj, Ref<LuaAPI> api) {
	if (obj->has_method("__metatable")) {
		return obj->call("__metatable", api);
	}

	return Variant();
}

Variant LuaDefaultObjectMetatable::__unm(Object *obj, Ref<LuaAPI> api) {
	if (obj->has_method("__unm")) {
		return obj->call("__unm", api);
	}

	return Variant();
}

Variant LuaDefaultObjectMetatable::__add(Object *obj, Ref<LuaAPI> api, Variant other) {
	if (obj->has_method("__add")) {
		return obj->call("__add", api, other);
	}

	return Variant();
}

Variant LuaDefaultObjectMetatable::__sub(Object *obj, Ref<LuaAPI> api, Variant other) {
	if (obj->has_method("__sub")) {
		return obj->call("__sub", api, other);
	}

	return Variant();
}

Variant LuaDefaultObjectMetatable::__mul(Object *obj, Ref<LuaAPI> api, Variant other) {
	if (obj->has_method("__mul")) {
		return obj->call("__mul", api, other);
	}

	return Variant();
}

Variant LuaDefaultObjectMetatable::__div(Object *obj, Ref<LuaAPI> api, Variant other) {
	if (obj->has_method("__div")) {
		return obj->call("__div", api, other);
	}

	return Variant();
}

Variant LuaDefaultObjectMetatable::__idiv(Object *obj, Ref<LuaAPI> api, Variant other) {
	if (obj->has_method("__idiv")) {
		return obj->call("__idiv", api, other);
	}

	return Variant();
}

Variant LuaDefaultObjectMetatable::__mod(Object *obj, Ref<LuaAPI> api, Variant other) {
	if (obj->has_method("__mod")) {
		return obj->call("__mod", api, other);
	}

	return Variant();
}

Variant LuaDefaultObjectMetatable::__pow(Object *obj, Ref<LuaAPI> api, Variant other) {
	if (obj->has_method("__pow")) {
		return obj->call("__pow", api, other);
	}

	return Variant();
}

Variant LuaDefaultObjectMetatable::__band(Object *obj, Ref<LuaAPI> api, Variant other) {
	if (obj->has_method("__band")) {
		return obj->call("__band", api, other);
	}

	return Variant();
}

Variant LuaDefaultObjectMetatable::__bor(Object *obj, Ref<LuaAPI> api, Variant other) {
	if (obj->has_method("__bor")) {
		return obj->call("__bor", api, other);
	}

	return Variant();
}

Variant LuaDefaultObjectMetatable::__bxor(Object *obj, Ref<LuaAPI> api, Variant other) {
	if (obj->has_method("__bxor")) {
		return obj->call("__bxor", api, other);
	}

	return Variant();
}

Variant LuaDefaultObjectMetatable::__bnot(Object *obj, Ref<LuaAPI> api) {
	if (obj->has_method("__bnot")) {
		return obj->call("__bnot", api);
	}

	return Variant();
}

Variant LuaDefaultObjectMetatable::__shl(Object *obj, Ref<LuaAPI> api, Variant other) {
	if (obj->has_method("__shl")) {
		return obj->call("__shl", api, other);
	}

	return Variant();
}

Variant LuaDefaultObjectMetatable::__shr(Object *obj, Ref<LuaAPI> api, Variant other) {
	if (obj->has_method("__shr")) {
		return obj->call("__shr", api, other);
	}

	return Variant();
}

Variant LuaDefaultObjectMetatable::__concat(Object *obj, Ref<LuaAPI> api, Variant other) {
	if (obj->has_method("__concat")) {
		return obj->call("__concat", api, other);
	}

	return Variant();
}

int LuaDefaultObjectMetatable::__len(Object *obj, Ref<LuaAPI> api) {
	if (obj->has_method("__len")) {
		return obj->call("__len", api);
	}

	return Variant();
}

bool LuaDefaultObjectMetatable::__eq(Object *obj, Ref<LuaAPI> api, Variant other) {
	if (obj->has_method("__eq")) {
		return obj->call("__eq", api, other);
	}

	return Variant();
}

bool LuaDefaultObjectMetatable::__lt(Object *obj, Ref<LuaAPI> api, Variant other) {
	if (obj->has_method("__lt")) {
		return obj->call("__lt", api, other);
	}

	return Variant();
}

bool LuaDefaultObjectMetatable::__le(Object *obj, Ref<LuaAPI> api, Variant other) {
	if (obj->has_method("__le")) {
		return obj->call("__le", api, other);
	}

	return Variant();
}
