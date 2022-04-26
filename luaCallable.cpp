#include "luaCallable.h"
#include "lua.h"
#include "core/templates/hashfuncs.h"


LuaCallable::LuaCallable(Ref<Lua> p_lua, int ref, lua_State *p_state){
    lua = p_lua;
    funcRef = ref; 
    state = p_state;
    h = (uint32_t)hash_djb2_one_64((uint64_t)this);
}

LuaCallable::~LuaCallable(){
}


bool LuaCallable::compare_equal(const CallableCustom *p_a, const CallableCustom *p_b) {
	// Lambda callables are only compared by reference.
	return p_a == p_b;
}

bool LuaCallable::compare_less(const CallableCustom *p_a, const CallableCustom *p_b) {
	// Lambda callables are only compared by reference.
	return p_a < p_b;
}


CallableCustom::CompareEqualFunc LuaCallable::get_compare_equal_func() const {
	return compare_equal;
}

CallableCustom::CompareLessFunc LuaCallable::get_compare_less_func() const {
	return compare_less;
}

ObjectID LuaCallable::get_object() const {
    return lua->get_instance_id();
}

String LuaCallable::get_as_text() const {
	// I dont know of a way to get a useful name from the function
	return vformat("lua function %X", h);
}

uint32_t LuaCallable::hash() const {
	return h;
}

void LuaCallable::call(const Variant **p_arguments, int p_argcount, Variant &r_return_value, Callable::CallError &r_call_error) const {
	Vector<Variant> args;
	args.resize(p_argcount);
	for (int i = 0; i < p_argcount; i++) {
		args.write[i] = *p_arguments[i];
	}
	
	lua_rawgeti(state, LUA_REGISTRYINDEX, funcRef);
	for (int i = 0; i < p_argcount; i++) {
		lua->pushVariant(args[i]);
	}
	int ret = lua_pcall(state, p_argcount, 1 , 0);
    if( ret != LUA_OK ){

        if( !lua->errorHandler.is_valid() ){
            print_error("Error during \"LuaCallable::call\"");
        } 
        lua->handleError( ret );
    }
	r_return_value = lua->getVariant(1);
	lua_pop(state, 1);
}