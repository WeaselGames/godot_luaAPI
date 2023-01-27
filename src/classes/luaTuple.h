#ifndef LUATUPLE_H
#define LUATUPLE_H
#include "core/object/ref_counted.h"
#include "core/core_bind.h"

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