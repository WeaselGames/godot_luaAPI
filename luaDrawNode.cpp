#include "luaDrawNode.h"

#include "core/math/math_funcs.h"
#include "scene/main/viewport.h"
#include "scene/resources/font.h"

LuaDrawNode::LuaDrawNode() {
    instance = nullptr;
    set_process(true);
    set_process_unhandled_key_input(true);
}

LuaDrawNode::~LuaDrawNode() {
    
}

void LuaDrawNode::_bind_methods(){
    ClassDB::bind_method(D_METHOD("bind_lua", "Lua", "ProtectedCall" , "CallbackCaller" , "Callback" ), &LuaDrawNode::luaBind, DEFVAL(true) , DEFVAL(Variant()) , DEFVAL(String()));
    ClassDB::bind_method(D_METHOD("set_font", "font"), &LuaDrawNode::setFont);
	ClassDB::bind_method(D_METHOD("get_font"), &LuaDrawNode::getFont);
    ClassDB::bind_method(D_METHOD("_unhandled_key_input"), &LuaDrawNode::unhandled_key_input);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "font", PROPERTY_HINT_RESOURCE_TYPE, "Font"), "set_font", "get_font");
}

void LuaDrawNode::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_PROCESS: {
			_process();
		} break;
        case NOTIFICATION_DRAW: {
            _draw();
        } break;
	}
}

void LuaDrawNode::unhandled_key_input(const Ref<InputEvent> &event){
    Ref<InputEventKey> inputKey = event;
    if (instance == nullptr) {
       return;
    }

    if (!instance->luaFunctionExists("_input")) {
        return;
    }
    Array args = Array();
    args.append((int)inputKey->get_keycode());
    args.append((int)inputKey->get_modifiers_mask());
    instance->callFunction("_input", args, protectedCall, callbackObj, callbackFunc);
}

void LuaDrawNode::_process() {
    if (instance == nullptr) {
       return;
    }

    if (!instance->luaFunctionExists("_process")) {
        return;
    }
    Array args = Array();
    args.append(get_process_delta_time());
    instance->callFunction("_process", args, protectedCall, callbackObj, callbackFunc);
}

void LuaDrawNode::_draw() {
    if (instance == nullptr) {
       return;
    }

    if (!instance->luaFunctionExists("_draw")) {
        return;
    }
    instance->callFunction("_draw", Array(), protectedCall, callbackObj, callbackFunc);
}

void LuaDrawNode::setFont(const Ref<Font> &pFont) {
	if (font == pFont) {
		return;
	}

	font = pFont;
}

Ref<Font> LuaDrawNode::getFont() {
	return font;
}

void LuaDrawNode::luaBind(Object *inst, bool protected_call , Object* callback_caller , String callback ) {
     
    instance = Object::cast_to<Lua>(inst);
    protectedCall = protected_call;
    callbackObj = callback_caller;
    callbackFunc = callback;
    bindDrawArc();
    bindDrawRect();
    bindDrawString();
    bindGetStringSize();
    bindUpdate();
    Vector2 size = get_viewport()->get_visible_rect().size;
    instance->pushGlobalVariant(size, "screensize");

}

void LuaDrawNode::bindDrawArc() {
    auto f = [](lua_State* L) -> int{
        class CanvasItem *node = (class CanvasItem*) lua_topointer(L, lua_upvalueindex(1));
        class Lua *obj = (class Lua*) lua_topointer(L, lua_upvalueindex(2));
        int argc = lua_gettop(L);
        Vector2 arg1 = obj->getVariant(1);
        float arg2 = obj->getVariant(2);
        float arg3 = obj->getVariant(3);
        float arg4 = obj->getVariant(4);
        int arg5 = obj->getVariant(5);
        Color arg6 = obj->getVariant(6);
        float arg7 = 1.0F;
        bool arg8 = false;
        if (argc > 6) {
            arg7 = obj->getVariant(7);
        }
        if (argc > 7) {
            arg8 = obj->getVariant(8);
        }
        node->draw_arc(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
        return 0;
    };
    // Pushing the referance of the class
    lua_pushlightuserdata(instance->getState(), this);

    // Pushing the object instnace to the stack to be retrived when the function is called
    lua_pushlightuserdata(instance->getState(), instance);

    // Pushing the actual lambda function to the stack
    lua_pushcclosure(instance->getState(), f, 2);

    // Setting the global name for the function in lua
    lua_setglobal(instance->getState(), "drawArc");
}

void LuaDrawNode::bindDrawRect() {
    auto f = [](lua_State* L) -> int{
        class CanvasItem *node = (class CanvasItem*) lua_topointer(L, lua_upvalueindex(1));
        class Lua *obj = (class Lua*) lua_topointer(L, lua_upvalueindex(2));
        int argc = lua_gettop(L);
        Vector2 arg1 = obj->getVariant(1);
        Vector2 arg2 = obj->getVariant(2);
        Color arg3 = obj->getVariant(3);
        bool arg4 =true;
        float arg5 = 1.0F;
        if (argc > 3) {
            arg4 = obj->getVariant(4);
        }
        if (argc > 4) {
            arg5 = obj->getVariant(5);
        }
        node->draw_rect(Rect2(arg1, arg2), arg3, arg4, arg5);
       
        return 0;
    };
    // Pushing the referance of the class
    lua_pushlightuserdata(instance->getState(), this);

    // Pushing the object instnace to the stack to be retrived when the function is called
    lua_pushlightuserdata(instance->getState(), instance);

    // Pushing the actual lambda function to the stack
    lua_pushcclosure(instance->getState(), f, 2);

    // Setting the global name for the function in lua
    lua_setglobal(instance->getState(), "drawRect");
}

void LuaDrawNode::bindDrawString() {
    auto f = [](lua_State* L) -> int{
        class CanvasItem *node = (class CanvasItem*) lua_topointer(L, lua_upvalueindex(1));
        class Lua *obj = (class Lua*) lua_topointer(L, lua_upvalueindex(2));
        Ref<Font> *pFont = (Ref<Font>*) lua_topointer(L, lua_upvalueindex(3));
        int argc = lua_gettop(L);
        Vector2 arg1 = obj->getVariant(1);
        String arg2 = obj->getVariant(2);
        
        int arg4 = 16;
        Color arg5 = Color(1, 1, 1);
        int arg6 = 0;
        Color arg7 = Color(1, 1, 1, 0);
        if (argc > 2) {
            arg4 = obj->getVariant(3);
        }
        if (argc > 3) {
            arg5 = obj->getVariant(4);
        }
        if (argc > 4) {
            arg6 = obj->getVariant(5);
        }
        if (argc > 5) {
            arg7 = obj->getVariant(6);
        }
        if (pFont->is_valid()) {
            node->draw_string(*pFont, arg1, arg2, HORIZONTAL_ALIGNMENT_LEFT, -1, arg4,  arg5, arg6, arg7, 3U);
        }
        return 0;
    };
    // Pushing the referance of the class
    lua_pushlightuserdata(instance->getState(), this);

    // Pushing the object instnace to the stack to be retrived when the function is called
    lua_pushlightuserdata(instance->getState(), instance);

    // Pushing the font to the stack
    lua_pushlightuserdata(instance->getState(), &font);

    // Pushing the actual lambda function to the stack
    lua_pushcclosure(instance->getState(), f, 3);

    // Setting the global name for the function in lua
    lua_setglobal(instance->getState(), "drawString");
}

void LuaDrawNode::bindGetStringSize() {
    auto f = [](lua_State* L) -> int{
        class Lua *obj = (class Lua*) lua_topointer(L, lua_upvalueindex(1));
        Ref<Font> *pFont = (Ref<Font>*) lua_topointer(L, lua_upvalueindex(2));
        int argc = lua_gettop(L);
        String arg1 = obj->getVariant(1);
        int arg2 = 16;
        if (argc > 1) {
            arg2 = obj->getVariant(2);
        }

        if (pFont->is_valid()) {
            Vector2 size = pFont->ptr()->get_string_size(arg1, arg2);
            obj->pushVariant(size);
            return 1;
        }
        return 0;
    };
    // Pushing the object instnace to the stack to be retrived when the function is called
    lua_pushlightuserdata(instance->getState(), instance);

    // Pushing the font to the stack
    lua_pushlightuserdata(instance->getState(), &font);

    // Pushing the actual lambda function to the stack
    lua_pushcclosure(instance->getState(), f, 2);

    // Setting the global name for the function in lua
    lua_setglobal(instance->getState(), "getStringSize");
}

void LuaDrawNode::bindUpdate() {
    auto f = [](lua_State* L) -> int{
        class CanvasItem *node = (class CanvasItem*) lua_topointer(L, lua_upvalueindex(1));
        node->update();
        return 0;
    };
    // Pushing the object instnace to the stack to be retrived when the function is called
    lua_pushlightuserdata(instance->getState(), this);

    // Pushing the actual lambda function to the stack
    lua_pushcclosure(instance->getState(), f, 1);

    // Setting the global name for the function in lua
    lua_setglobal(instance->getState(), "update");
}