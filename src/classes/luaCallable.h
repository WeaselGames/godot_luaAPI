#ifndef LAPI_GDEXTENSION

#ifndef LUACALLABLE_H
#define LUACALLABLE_H

#include "core/object/ref_counted.h"
#include "core/variant/callable.h"

#include <classes/luaAPI.h>

#include <lua/lua.hpp>

#ifdef LAPI_GDEXTENSION
using namespace godot;
#endif

class LuaCallable : public CallableCustom {
	static bool compare_equal(const CallableCustom *p_a, const CallableCustom *p_b);
	static bool compare_less(const CallableCustom *p_a, const CallableCustom *p_b);
	uint32_t h;

public:
	uint32_t hash() const override;
	String get_as_text() const override;
	CompareEqualFunc get_compare_equal_func() const override;
	CompareLessFunc get_compare_less_func() const override;
	ObjectID get_object() const override;
	lua_State *getLuaState() const;
	void call(const Variant **p_argument, int p_argcount, Variant &r_return_value, Callable::CallError &r_call_error) const override;

	int getFuncRef();

	LuaCallable(Ref<LuaAPI> obj, int ref, lua_State *p_state);
	~LuaCallable() override;

private:
	int funcRef;
	Ref<LuaAPI> obj;
	lua_State *state = nullptr;
};

#endif
#endif