#ifndef LUADRAWNODE_H
#define LUADRAWNODE_H

#include "scene/2d/node_2d.h"
#include "lua.h"

class LuaDrawNode : public Node2D {
    GDCLASS(LuaDrawNode, Node2D);
    
    

protected:
    static void _bind_methods();
    void _notification(int p_what);
    void unhandled_key_input(const Ref<InputEvent> &p_key_event);

private:
    void _process();
    void _draw();
    void bindDrawArc();
    void bindDrawRect();
    void bindDrawString();
    void bindGetStringSize();
    void bindUpdate();

public:
    LuaDrawNode();
    ~LuaDrawNode();
    void luaBind(Object *instance, bool protected_call = true , Object* CallbackCaller = nullptr , String callback = String() );
    void setFont(const Ref<Font> &pFont);
    Ref<Font> getFont();

    // Lua functions
    
private:
    Lua *instance;
    bool protectedCall;
    Object* callbackObj;
    String callbackFunc;
    Ref<Font> font;

    float rotAg = 50;
    float agFrom = 75;
    float agTo = 195;
    

}; 


#endif