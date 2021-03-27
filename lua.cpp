
#include "lua.h"

Lua::Lua(){
  state = luaL_newstate();
  luaL_openlibs(state);

  lua_register(state, "print", luaPrint);
}

Lua::~Lua(){
  lua_close(state);
}

void Lua::_bind_methods(){
  ClassDB::bind_method(D_METHOD("run", "code"), &Lua::run);
  ClassDB::bind_method("execute", &Lua::execute);
  ClassDB::bind_method("load",&Lua::load);
  ClassDB::bind_method("pushVariant",&Lua::pushGlobalVariant);
  ClassDB::bind_method(D_METHOD("exposeFunction", "NodeObject", "GDFunction", "LuaFunctionName"),&Lua::exposeFunction);
}

void Lua::exposeFunction(Object *instance, String function, String name){
  
  auto f = [](lua_State* state) -> int{
    const Object *instance2 = (const Object*) lua_topointer(state, lua_upvalueindex(1));
    const char *function2 = (const char*) lua_tostring(state, lua_upvalueindex(2));
    ScriptInstance *scriptInstance = instance2->get_script_instance();
    scriptInstance->call(function2);

    return 0;
  };

  lua_pushlightuserdata(state, instance);
  std::wstring temp = function.c_str();
  const char *func = std::string ( temp.begin(), temp.end() ).c_str();

  std::wstring ntemp = name.c_str();
  const char *fname = std::string ( ntemp.begin(), ntemp.end() ).c_str();

  lua_pushstring(state, func);
  lua_pushcclosure(state, f, 2);
  lua_setglobal(state, fname);
  
}

bool Lua::load(String fileName) {
    _File file;

    file.open(fileName,_File::ModeFlags::READ);

    String text = file.get_as_text();
    Vector<String> lines = text.split("\n");
    std::map<String,String> allFunctions;
    String inFunction = "";
    String functionBegin = "function";
    String functionEnd = "end";

    for (int i = 0; i < lines.size(); i++) {
        String line = lines[i];
        if (inFunction == "") {
            if (line.begins_with(functionBegin)) {

                int endPos = line.find("(");
                String name = line.substr(functionBegin.length(),endPos-functionBegin.length()).strip_edges();               
                allFunctions.insert(std::pair<String,String>(name,line+"\n"));
                inFunction = name;
            }
        } else {
            allFunctions[inFunction]+=(line+"\n");
            if (line == functionEnd) {
                inFunction = "";
            }
        }
    }

    for(std::pair<String,String> function : allFunctions) {
      std::wstring luaCode = function.second.c_str();

        if (luaL_loadstring(state, std::string( luaCode.begin(), luaCode.end() ).c_str()) || (lua_pcall(state, 0, 0 , 0))) {
            print_line("Error: script not loaded (" + function.first + ")");
            state = 0;
            return false;
        }
    }
    file.close();
    return true;
}

Variant Lua::execute(String name, Array array) {
    std::wstring luaCode =  name.c_str();
    lua_getglobal(state, std::string( luaCode.begin(), luaCode.end() ).c_str());
    for (int i = 0; i < array.size(); i++) {
        Variant var = array[i];
        pushVariant(var);
    }

    lua_pcall(state, array.size(), LUA_MULTRET, 0);
    int numReturns = lua_gettop(state);

    if (numReturns) {
        Array results;
        for(int i = 0; i < numReturns; i++) {
            results.append(popVariant());
        }
        return results;
    } else {
        return Variant();
    }
}

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

bool Lua::pushGlobalVariant(Variant var, String name) {
    if (pushVariant(var)) {
       std::wstring str = name.c_str();
        lua_setglobal(state,std::string( str.begin(), str.end() ).c_str());
        return true;
    }
    return false;
}

Variant Lua::popVariant() {
    Variant result = getVariant();
    lua_pop(state, 1);
    return result;
}

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

void Lua::run(String code) {

  int result;

  std::wstring luaCode = code.c_str();
  result = luaL_loadstring(state, std::string( luaCode.begin(), luaCode.end() ).c_str());

  if ( result != LUA_OK ) {
    print_line("Rip1!");
    return;
  }

  result = lua_pcall(state, 0, LUA_MULTRET, 0);

  if ( result != LUA_OK ) {
    print_line("Rip!2");
    return;
  }
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