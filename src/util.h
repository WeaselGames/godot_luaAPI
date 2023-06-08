#ifndef UTIL_H
#define UTIL_H

#ifndef LAPI_GDEXTENSION
#include <core/string/print_string.h>
#include <core/variant/array.h>

inline Array get_all_scripts_from_object(const Object const* obj) {
	Array a;
	if (obj == null) return a;
	Variant script = obj.get_script();
	while (script != nullptr) {
		if (script.get_type() != Variant.Type.OBJECT) {
			break; //not a script
		}
		a.append(script);
	}
	return a;
}

#else
#error cannot find array.h for gdextension //TODO: fix it dammit
#include <godot_cpp/variant/utility_functions.hpp>
using namespace godot;

inline void print_line(const Variant &v) {
	UtilityFunctions::print(v);
}
#endif
#endif
