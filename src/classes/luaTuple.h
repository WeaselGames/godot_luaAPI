#ifndef LUATUPLE_H
#define LUATUPLE_H

#ifndef LAPI_GODOT_EXTENSION
#include "core/object/ref_counted.h"
#include "core/core_bind.h"
#else
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/callable.hpp>
#endif

#ifdef LAPI_GODOT_EXTENSION
using namespace godot;
#endif

class LuaTuple : public RefCounted {
    GDCLASS(LuaTuple, RefCounted);

    protected:
        static void _bind_methods();
    
    public:
        static LuaTuple* fromArray(Array elms);

        void pushBack(Variant var);
        void pushFront(Variant var);
        void set(int i, Variant var);
        void clear();

        bool isEmpty();

        int size();

        Variant popBack();
        Variant popFront();
        Variant get(int i);

        Array toArray();
    private:
        Array elements;
};
#endif