#ifndef LAPI_GDEXTENSION

#include "luaCallable.h"
#include "luaAPI.h"

#include "core/templates/hashfuncs.h"

// I used "GDScriptLambdaCallable" as a template for this
LuaCallable::LuaCallable(Ref<LuaAPI> p_obj, int ref, lua_State *p_state) {
	obj = p_obj;
	funcRef = ref;
	state = p_state;
	h = (uint32_t)hash_djb2_one_64((uint64_t)this);
}

LuaCallable::~LuaCallable() {
	print_line("LuaCallable::~LuaCallable()");
	luaL_unref(state, LUA_REGISTRYINDEX, funcRef);
}

bool LuaCallable::compare_equal(const CallableCustom *p_a, const CallableCustom *p_b) {
	// Lua callables are only compared by reference.
	return p_a == p_b;
}

bool LuaCallable::compare_less(const CallableCustom *p_a, const CallableCustom *p_b) {
	// Lua callables are only compared by reference.
	return p_a < p_b;
}

CallableCustom::CompareEqualFunc LuaCallable::get_compare_equal_func() const {
	return compare_equal;
}

CallableCustom::CompareLessFunc LuaCallable::get_compare_less_func() const {
	return compare_less;
}

ObjectID LuaCallable::get_object() const {
	return obj->get_instance_id();
}

String LuaCallable::get_as_text() const {
	// I dont know of a way to get a useful name from the function
	// For now we are just using the callables hash.
	return vformat("luaCallable 0x%X", h);
}

lua_State *LuaCallable::getLuaState() const {
	return state;
}

uint32_t LuaCallable::hash() const {
	return h;
}

void LuaCallable::call(const Variant **p_arguments, int p_argcount, Variant &r_return_value, Callable::CallError &r_call_error) const {
	lua_pushcfunction(state, LuaState::luaErrorHandler);

	// Getting the lua function via the reference stored in funcRef
	lua_rawgeti(state, LUA_REGISTRYINDEX, funcRef);

	// ------------------
	// This is a hack to match the API with the GDExtension Callable workaround
	if (p_argcount != 1 || p_arguments[0]->get_type() != Variant::Type::ARRAY) {
		r_return_value = LuaError::newError("LuaCallable arguments must be supplied with a Godot Array", LuaError::ERR_TYPE);
		return;
	}

	Array args = p_arguments[0]->operator Array();
	for (int i = 0; i < args.size(); i++) {
		LuaState::pushVariant(state, args[i]);
	}

	p_argcount = args.size();
	// ------------------

	// Push all the argument on to the stack
	// for (int i = 0; i < p_argcount; i++) {
	// 	LuaState::pushVariant(state, *p_arguments[i]);
	// }

	// execute the function using a protected call.
	int ret = lua_pcall(state, p_argcount, 1, -2 - p_argcount);
	if (ret != LUA_OK) {
		r_return_value = LuaState::handleError(state, ret);
	} else {
		r_return_value = LuaState::getVariant(state, -1);
	}

	lua_pop(state, 1);
}

int LuaCallable::getFuncRef() {
	return funcRef;
}
#endif