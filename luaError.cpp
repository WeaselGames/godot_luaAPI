#include "luaError.h"

void LuaError::_bind_methods(){
    // Binding static GD methods. This allows the use of LuaError.new_error instead of neededing to call LuaError.new and than calling .setInfo
    ClassDB::bind_static_method("LuaError", D_METHOD("new_err", "Message", "Type"), &LuaError::newErr, DEFVAL(LuaError::ERR_RUNTIME));
    
    ClassDB::bind_method(D_METHOD("set_msg","Message"), &LuaError::setMsg);
    ClassDB::bind_method(D_METHOD("get_msg"), &LuaError::getMsg);
    ClassDB::bind_method(D_METHOD("set_type","Type"), &LuaError::setType);
    ClassDB::bind_method(D_METHOD("get_type"), &LuaError::getType);
    
    ADD_PROPERTY(PropertyInfo(Variant::INT, "type"), "set_type", "get_type");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "msg"), "set_msg", "get_msg");

    BIND_ENUM_CONSTANT(ERR_TYPE);
    BIND_ENUM_CONSTANT(ERR_RUNTIME);
    BIND_ENUM_CONSTANT(ERR_SYNTAX);
    BIND_ENUM_CONSTANT(ERR_MEMORY);
    BIND_ENUM_CONSTANT(ERR_ERR);
    BIND_ENUM_CONSTANT(ERR_FILE);
}

// Create a new error
LuaError* LuaError::newErr(String msg, ErrorType type) {
    LuaError* err = memnew(LuaError);
    err->setInfo(msg, static_cast<LuaError::ErrorType>(type));
    return err;
}

void LuaError::setInfo(String msg, ErrorType type) {
    errType = type;
    errMsg = msg;
}

bool LuaError::operator==(const ErrorType type) {
    return errType == type;
}

bool LuaError::operator==(const LuaError err) {
    return errType == err.getType();
}

void LuaError::setMsg(String msg) {
    errMsg = msg;
}

String LuaError::getMsg() const {
    return errMsg;
}

void LuaError::setType(ErrorType type) {
    errType = type;
}

LuaError::ErrorType LuaError::getType() const {
    return errType;
}