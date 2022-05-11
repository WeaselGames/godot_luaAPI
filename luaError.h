#ifndef LUAERROR_H
#define LUAERROR_H

#include "core/object/ref_counted.h"
#include "core/core_bind.h"
#include "luasrc/lua.hpp"

class LuaError : public RefCounted {
    GDCLASS(LuaError, RefCounted);

    protected:
        static void _bind_methods();

    public:
        enum ErrorType{
            ERR_NONE    = 0,
            ERR_TYPE    = 1,
            ERR_RUNTIME = LUA_ERRRUN,
            ERR_SYNTAX  = LUA_ERRSYNTAX,
            ERR_MEMORY  = LUA_ERRMEM,
            ERR_ERR     = LUA_ERRERR,
            ERR_FILE    = LUA_ERRFILE,
        };
        static LuaError* newError(String msg, ErrorType type);
        static LuaError* errNone();
        static bool isErr(Variant var);

        void setInfo(String msg, ErrorType type);
        operator String() const;
        bool operator==(const ErrorType type);

        void setMsg(String msg);
        String getMsg();
        void setType(ErrorType type);
        ErrorType getType();
        int getLineNumber();

    private:
        ErrorType errType;
        String errMsg;
};

VARIANT_ENUM_CAST(LuaError::ErrorType)

#endif