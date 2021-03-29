
#include "lua.h"

#include <thread>

Lua::Lua(){
  // Createing lua state instance
  state = luaL_newstate();
  threaded = true;
  luaopen_base(state);
  luaopen_math(state);
  luaopen_string(state);
  luaopen_table(state);

  lua_register(state, "print", luaPrint);
}

Lua::~Lua(){
  // Destroying lua state instance
  lua_close(state);
}

// Bind C++ functions to GDScript
void Lua::_bind_methods(){
    ClassDB::bind_method(D_METHOD("setThreaded", "bool"),&Lua::setThreaded);
    ClassDB::bind_method(D_METHOD("doFile", "NodeObject", "File", "Callback=String()"), &Lua::doFile);
    ClassDB::bind_method(D_METHOD("doString", "NodeObject", "Code", "Callback=String()"), &Lua::doString);
    ClassDB::bind_method(D_METHOD("pushVariant", "var"),&Lua::pushGlobalVariant);
    ClassDB::bind_method(D_METHOD("exposeFunction", "NodeObject", "GDFunction", "LuaFunctionName"),&Lua::exposeFunction);
}

// expose a GDScript function to lua
void Lua::exposeFunction(Object *instance, String function, String name){
  
  // Createing lamda function so we can capture the object instanse and call the GDScript method. Or in theory other scripting languages?
  auto f = [](lua_State* L) -> int{
    const Object *instance2 = (const Object*) lua_topointer(L, lua_upvalueindex(1));
    class Lua *obj = (class Lua*) lua_topointer(L, lua_upvalueindex(2));
    const char *function2 = lua_tostring(L, lua_upvalueindex(3));

    Variant arg1 = obj->getVariant(1);
    Variant arg2 = obj->getVariant(2);
    Variant arg3 = obj->getVariant(3);
    Variant arg4 = obj->getVariant(4);
    Variant arg5 = obj->getVariant(5);

    ScriptInstance *scriptInstance = instance2->get_script_instance();
    Variant returned = scriptInstance->call(function2, arg1, arg2, arg3, arg4, arg5);

    if (returned.get_type() != Variant::Type::NIL){
      obj->pushVariant(returned);
      return 1;
    }
    return 0;
  };

  // Pushing the object instnace to the stack to be retrived when the function is called
  lua_pushlightuserdata(state, instance);

  // Pushing the referance of the class
  lua_pushlightuserdata(state, this);

  // You will see this style of casting a lot as it is the best way I was able to find to conver from Godot's String class to a const cahr *
  std::wstring temp = function.c_str();
  const char *func = std::string ( temp.begin(), temp.end() ).c_str();

  std::wstring ntemp = name.c_str();
  const char *fname = std::string ( ntemp.begin(), ntemp.end() ).c_str();

  // Pushing the script function name string to the stack to br retrived when called
  lua_pushstring(state, func);

  // Pushing the actual lambda function to the stack
  lua_pushcclosure(state, f, 3);
  // Setting the global name for the function in lua
  lua_setglobal(state, fname);
  
}

void Lua::setThreaded(bool thread){
    threaded = thread;
}

// doFile() will just load the file's text and call doString()
void Lua::doFile(Object *instance, String fileName, String callback){
  _File file;

  file.open(fileName,_File::ModeFlags::READ);
  String code = file.get_as_text();
  file.close();

  doString(instance, code, callback);
}

// Run lua string in a thread
void Lua::doString(Object *instance, String code, String callback){
    if(threaded){
        std::thread (runLua, instance, code, callback, state).detach();
    }else{
        runLua(instance, code, callback, state);
    }
}

// Execute a lua script string and call the passed callBack function with the error as the aurgument if an error occures
void Lua::runLua(Object *instance, String code, String callback, lua_State *L){
    std::wstring luaCode = code.c_str();

    int result = luaL_dostring(L, std::string( luaCode.begin(), luaCode.end() ).c_str());
    if(result != LUA_OK){
        if (callback != String()){
            ScriptInstance *scriptInstance = instance->get_script_instance();
            scriptInstance->call(callback, lua_tostring(L, -1));
        }
    }
}

// Push a GD Variant to the lua stack and return false if type is not supported.
bool Lua::pushVariant(Variant var) {
    std::wstring str;
    switch (var.get_type())
    {
        case Variant::Type::STRING:
            str = (var.operator String().c_str());
            lua_pushstring(state, std::string( str.begin(), str.end() ).c_str());
            break;
        case Variant::Type::INT:
            lua_pushinteger(state, (int64_t)var);
            break;
        case Variant::Type::REAL:
            lua_pushnumber(state,var.operator double());
            break;
        case Variant::Type::BOOL:
            lua_pushboolean(state, (bool)var);
            break;
        case Variant::Type::POOL_BYTE_ARRAY:
        case Variant::Type::POOL_INT_ARRAY:
        case Variant::Type::POOL_STRING_ARRAY:
        case Variant::Type::POOL_REAL_ARRAY:
        case Variant::Type::POOL_VECTOR2_ARRAY:
        case Variant::Type::POOL_VECTOR3_ARRAY:
        case Variant::Type::POOL_COLOR_ARRAY:            
        case Variant::Type::ARRAY: {
            Array array = var.operator Array();
            lua_newtable(state);
            for(int i = 0; i < array.size(); i++) {
                Variant key = i+1;
                Variant value = array[i];
                pushVariant(key);
                pushVariant(value);
                lua_settable(state,-3);
            }
            break;
        }
        case Variant::Type::DICTIONARY:
            lua_newtable(state);
            for(int i = 0; i < ((Dictionary)var).size(); i++) {
                Variant key = ((Dictionary)var).keys()[i];
                Variant value = ((Dictionary)var)[key];
                pushVariant(key);
                pushVariant(value);
                lua_settable(state, -3);
            }
            break;
        case Variant::Type::VECTOR2: {
            Vector2 vector2 = var.operator Vector2();
            lua_newtable(state);
            lua_pushstring(state, "X");
            lua_pushnumber(state, vector2.x);
            lua_settable(state, -3);
            lua_pushstring(state, "Y");
            lua_pushnumber(state, vector2.y);
            lua_settable(state, -3);
            break;     
        }     
        case Variant::Type::VECTOR3: {
            Vector3 vector3 = var.operator Vector3();
            lua_newtable(state);
            lua_pushstring(state,"X");
            lua_pushnumber(state,vector3.x);
            lua_settable(state,-3);
            lua_pushstring(state,"Y");
            lua_pushnumber(state,vector3.y);
            lua_settable(state,-3);
            lua_pushstring(state,"Z");
            lua_pushnumber(state,vector3.z);
            lua_settable(state,-3);
            break; 
        }
        case Variant::Type::COLOR: {
            Color color = var.operator Color();
            lua_newtable(state);
            lua_pushstring(state,"R");
            lua_pushnumber(state,color.r);
            lua_settable(state,-3);
            lua_pushstring(state,"G");
            lua_pushnumber(state,color.g);
            lua_settable(state,-3);
            lua_pushstring(state,"B");
            lua_pushnumber(state,color.b);
            lua_settable(state,-3);
            lua_pushstring(state,"A");
            lua_pushnumber(state,color.a);
            lua_settable(state,-3);             
            break;
        }
        default:
            print_line(var);
            return false;
    }
    return true;
}

// Call pushVarient() and set it to a global name
bool Lua::pushGlobalVariant(Variant var, String name) {
    if (pushVariant(var)) {
       std::wstring str = name.c_str();
        lua_setglobal(state,std::string( str.begin(), str.end() ).c_str());
        return true;
    }
    return false;
}

// Pop the value at the top of the stack and return getVarient()
Variant Lua::popVariant() {
    Variant result = getVariant();
    lua_pop(state, 1);
    return result;
}

// get a value at the given index and return as a variant
Variant Lua::getVariant(int index) {
    Variant result;
    int type = lua_type(state, index);
    switch (type) {
        case LUA_TSTRING:
            result = lua_tostring(state, index);
            break;
        case LUA_TNUMBER:
            result = lua_tonumber(state, index);
            break;
        case LUA_TBOOLEAN:
            result = (bool)lua_toboolean(state, index);
            break;
        case LUA_TTABLE:
        {
            Dictionary dict;
            for (lua_pushnil(state); lua_next(state, index-1); lua_pop(state, 1)) {
                Variant key = getVariant(-2);
                Variant value = getVariant(-1);
                dict[key] = value;
            }
            if (dict.size() == 2) {
                if (dict.keys().count("X") && dict.keys().count("Y")) {
                    result = Vector2(dict["X"],dict["Y"]);
                }
            } else if (dict.size() == 3) {
                if (dict.keys().count("X") && dict.keys().count("Y") && dict.keys().count("Z")) {
                    result = Vector3(dict["X"],dict["Y"],dict["Z"]);
                }
            } else if (dict.size() == 4) {
                if (dict.keys().count("R") && dict.keys().count("G") && dict.keys().count("B") && dict.keys().count("B")) {
                    result = Color(dict["R"],dict["G"],dict["B"],dict["A"]);
                }
            } else {
                result = dict;
            }
            break;
        }
        default:
            result = Variant();
    }
    return result;
}


// Lua functions

// Change lua's print function to print to the Godot console by default
int Lua::luaPrint(lua_State* state)
{

  int args = lua_gettop(state);

  for ( int n=1; n<=args; ++n) {
    print_line(lua_tostring(state, n));
  }

  return 0;
}
