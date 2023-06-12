#ifndef UTIL_H
#define UTIL_H

#ifndef LAPI_GDEXTENSION
#include <core/string/print_string.h>
#include <core/variant/array.h>

#else
#include <godot_cpp/variant/utility_functions.hpp>
using namespace godot;

inline void print_line(const Variant &v) {
	UtilityFunctions::print(v);
}
#endif
inline Array get_all_scripts_from_object(const Object const *obj) {
	Array a;
	if (obj == nullptr)
		return a;
	Variant script = obj->get_script();
	if (!script.is_null()) {
		while (!script.is_null()) {
			if (script.get_type() != Variant::Type::OBJECT) {
				break; //not a script
			}
			a.append(script);
		}
	}
	return a;
}
#endif
