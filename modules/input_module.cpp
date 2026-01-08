#include "input_module.h"
#include "common.h"
#include <cmath>
#include <mutex>

#include "oxlib/math.h"
#include "window_module.h"

namespace ox {OX_TYPEDEF(bool,boolean);};

namespace input {
    using namespace ox;
    
    static constexpr int MAX_KEYS = 512;
    static constexpr int MAX_BUTTON = 8;

    std::mutex _input_lock;

    static ox::boolean keyCurrent[MAX_KEYS] = { false };
    static ox::boolean keyPrevious[MAX_KEYS] = { false };
    static ox::boolean keyRepeat[MAX_KEYS] = { false };
           
    static ox::boolean mouseCurrent[MAX_BUTTON] = { false };
    static ox::boolean mousePrevious[MAX_BUTTON] = { false };
    static ox::boolean mouseRepeat[MAX_BUTTON] = { false };

    static i16 mouseX = 0, mouseY = 0;
    static i16 prevMouseX = 0, prevMouseY = 0;
    static i16 deltaX = 0, deltaY = 0;

    static i8 wheelDelta = 0;

    // =====================================================================
    // FRAME UPDATE
    // =====================================================================

    void _NewFrame() {
        std::scoped_lock lock(_input_lock);
        for (int i = 0; i < MAX_KEYS; i++) {
            keyPrevious[i] = keyCurrent[i];
        }
        for (int i = 0; i < MAX_BUTTON; i++) {
            mousePrevious[i] = mouseCurrent[i];
        }

        deltaX = deltaY = 0;
        wheelDelta = 0;

        prevMouseX = mouseX;
        prevMouseY = mouseY;
    }

    //!! call on main thread
    void InputNewFrame() {
        _NewFrame();
    }

    typedef ox::u64 keycode;

    ox::boolean IsKeyDown(const keycode code) {
        std::scoped_lock lock(_input_lock);
        return keyCurrent[code];
    }
    ox::boolean IsKeyJustDown(const keycode code) {
        std::scoped_lock lock(_input_lock);
        return keyCurrent[code] && !keyPrevious[code];
    }

    ox::boolean IsKeyUp(const keycode code) {
        std::scoped_lock lock(_input_lock);
        return !keyCurrent[code] && keyPrevious[code];
    }

    ox::boolean IsKeyRepeat(const keycode code) {
        return keyRepeat[code];
    }

    ox::boolean IsKeyPressed(const keycode code) {
        return keyCurrent[code];
    }



    // =====================================================================
    // MOUSE API
    // =====================================================================

    typedef u8 mouse_button;

    ox::boolean IsMouseButtonDown(const mouse_button button) {
        return mouseCurrent[button];
    }

    ox::boolean IsMouseButtonJustDown(const mouse_button button) {
        return mouseCurrent[button] && !mousePrevious[button];
    }

    ox::boolean IsMouseButtonUp(const mouse_button button) {
        return !mouseCurrent[button] && mousePrevious[button];
    }

    ox::boolean IsMouseButtonRepeat(const mouse_button button) {
        return mouseRepeat[button];
    }

    ox::boolean IsMouseButtonPressed(const mouse_button button) {
        return mouseCurrent[button];
    }


    // Mouse position
    ox::result GetMousePos(i16& xpos, i16& ypos) {
        xpos = mouseX;
        ypos = mouseY;
        return ox::ok;
    }

    // Mouse delta
    ox::result GetMouseDelta(i16& dx, i16& dy) {
        dx = deltaX;
        dy = deltaY;
        return ox::ok;
    }

    // Wheel
    ox::result GetMouseWheel(i8& force) {
        if (wheelDelta == 0) return ox::err;
        force = wheelDelta;
        return ox::ok;
    }



    // =====================================================================
    // INTERNAL BACKEND CALLBACKS
    // Called by OS layer (ex: GLFW key callback)
    // =====================================================================
    namespace input_internal {

        void OnKeyEvent(const int key, const int action) {
            if (key < 0 || key >= MAX_KEYS)
                return;

            if (action == 1) {          // PRESS
                keyCurrent[key] = true;
            }
            else if (action == 0) {   // RELEASE
                keyCurrent[key] = false;
                keyRepeat[key] = false;
            }
            else if (action == 2) {   // REPEAT
                keyRepeat[key] = true;
            }
        }

        void OnMouseButtonEvent(const int button, const int action) {
            if (button < 0 || button >= MAX_BUTTON)
                return;

            if (action == 1) {          // PRESS
                mouseCurrent[button] = true;
            }
            else if (action == 0) {   // RELEASE
                mouseCurrent[button] = false;
            }
            else if (action == 2) {   // REPEAT
                mouseRepeat[button] = true;
            }
        }

        void OnMouseMove(const i16 xpos, const i16 ypos) {
            mouseX = xpos;
            mouseY = ypos;

            deltaX = mouseX - prevMouseX;
            deltaY = mouseY - prevMouseY;
        }

        void OnMouseScroll(i8 delta) {
            wheelDelta = delta;
        }
    }

}


ox::result input_module::init(){
	this->Logger = dynamic_cast<logger*>(Managers[0]);
	this->Modules = dynamic_cast<module_manager*>(Managers[3]);

	auto winmod = (window_module*)Modules->get_modules()[32];

	if(!winmod){
		Logger->error("cannot find dependency WindowModule, is something went wrong?");
	}else{
		winmod->PushKeyEvent("_InputModule_ID_0001", input::input_internal::OnKeyEvent);
		winmod->PushMouseButtonEvent("_InputModule_ID_0001", input::input_internal::OnMouseButtonEvent);
		winmod->PushMouseMoveEvent("_InputModule_ID_0001", input::input_internal::OnMouseMove);
		winmod->PushMouseScrollEvent("_InputModule_ID_0001", input::input_internal::OnMouseScroll);
	}
	return ox::ok;
}
ox::result input_module::update(double dt){
    input::InputNewFrame();
	return ox::ok;
}
ox::result input_module::shutdown(){
	return ox::ok;
}

ox::vec2s input_module::GetMousePos(){
    ox::vec2s _v;
    input::GetMousePos(_v.x(), _v.y());
    return _v;
}
bool input_module::IsMouseDown(ox::i8 button){
    return input::IsMouseButtonDown(button);
}
double input_module::MouseYScroll(){
    ox::i8 _dt;
    input::GetMouseWheel(_dt);
    return _dt;
}

bool input_module::IsKeyJustDown(ox::i32 keycode){
    return input::IsKeyJustDown(keycode);
}
bool input_module::IsKeyDown(ox::i32 keycode){
    return input::IsKeyDown(keycode);
}

bool input_module::IsMouseJustDown(ox::i8 code){
    return input::IsMouseButtonJustDown(code);
}


ox::vec2f input_module::GetMouseDelta(){
    ox::vec2s f;
    input::GetMouseDelta(f.x(), f.y());
    return {(float)f.x(),(float)f.y()};
}
