#include "luaError.h"

LuaError* LuaError::newError(String msg, ErrorType type) {
    LuaError* err = memnew(LuaError);
    err->setInfo(msg, static_cast<LuaError::ErrorType>(type));
    return err;
}

LuaError* LuaError::errNone() {
    LuaError* err = memnew(LuaError);
    err->setInfo("", ErrorType::ERR_NONE);
    return err;
}

void LuaError::setInfo(String msg, ErrorType type) {
    errType = type;
    errMsg = msg;
}

LuaError::operator String() const {
    return errMsg;
}

bool LuaError::operator==(const ErrorType type) {
    return errType == type;
}

void LuaError::setMsg(String msg) {
    errMsg = msg;
}

String LuaError::getMsg() {
    return errMsg;
}

void LuaError::setType(ErrorType type) {
    errType = type;
}

LuaError::ErrorType LuaError::getType() {
    return errType;
}

void LuaError::_bind_methods(){
    ClassDB::bind_static_method("LuaError", D_METHOD("new_error", "Message", "Type"), &LuaError::newError);
    ClassDB::bind_method(D_METHOD("set_msg","Message"), &LuaError::setMsg);
    ClassDB::bind_method(D_METHOD("get_msg"), &LuaError::getMsg);
    ClassDB::bind_method(D_METHOD("set_type","Type"), &LuaError::setType);
    ClassDB::bind_method(D_METHOD("get_type"), &LuaError::getType);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "type"), "set_type", "get_type");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "msg"), "set_msg", "get_msg");

    BIND_ENUM_CONSTANT(ERR_NONE);
    BIND_ENUM_CONSTANT(ERR_TYPE);
    BIND_ENUM_CONSTANT(ERR_RUNTIME);
    BIND_ENUM_CONSTANT(ERR_SYNTAX);
    BIND_ENUM_CONSTANT(ERR_MEMORY);
    BIND_ENUM_CONSTANT(ERR_ERR);
    BIND_ENUM_CONSTANT(ERR_FILE);
}