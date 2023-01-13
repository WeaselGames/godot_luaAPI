#include "luaCallable.h"
#include "luaAPI.h"
#include "core/templates/hashfuncs.h"

// I used "GDScriptLambdaCallable" as a template for this
LuaCallable::LuaCallable(Ref<RefCounted> p_obj, int ref, lua_State *p_state){
    obj = p_obj;
    funcRef = ref; 
    state = p_state;
    h = (uint32_t)hash_djb2_one_64((uint64_t)this);
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

uint32_t LuaCallable::hash() const {
	return h;
}

void LuaCallable::call(const Variant **p_arguments, int p_argcount, Variant &r_return_value, Callable::CallError &r_call_error) const {
	// Geting the lua function via the referance stored in funcRef
	lua_rawgeti(state, LUA_REGISTRYINDEX, funcRef);

	// Push all the argument on to the stack
	for (int i = 0; i < p_argcount; i++) {
		LuaAPI::pushVariant(*p_arguments[i], state);
	}

	// execute the function using a protected call.
	int ret = lua_pcall(state, p_argcount, 1, 0);
    if (ret != LUA_OK) {
        r_return_value = LuaAPI::handleError(ret, state);
    } else r_return_value = LuaAPI::getVariant(1, state, obj);

	lua_pop(state, 1);
}

int LuaCallable::getFuncRef() {
	return funcRef;
}