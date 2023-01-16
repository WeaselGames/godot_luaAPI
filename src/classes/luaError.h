#ifndef LUAERROR_H
#define LUAERROR_H

#include "core/object/ref_counted.h"
#include "core/core_bind.h"

#include "../lua.hpp"

class LuaError : public RefCounted {
    GDCLASS(LuaError, RefCounted);

    protected:
        static void _bind_methods();

    public:
        enum ErrorType{
            ERR_TYPE    = 1,
            ERR_RUNTIME = LUA_ERRRUN,
            ERR_SYNTAX  = LUA_ERRSYNTAX,
            ERR_MEMORY  = LUA_ERRMEM,
            ERR_ERR     = LUA_ERRERR,
            ERR_FILE    = LUA_ERRFILE,
        };
        static LuaError* newErr(String msg, ErrorType type);

        void setInfo(String msg, ErrorType type);
        bool operator==(const ErrorType type);
        bool operator==(const LuaError err);

        void setMsg(String msg);
        String getMsg() const;
        void setType(ErrorType type);
        ErrorType getType() const;

    private:
        ErrorType errType;
        String errMsg;
};

VARIANT_ENUM_CAST(LuaError::ErrorType)

#endif