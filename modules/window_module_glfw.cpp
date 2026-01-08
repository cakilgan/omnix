#include "window_module.h"
#include <GLFW/glfw3.h>
#include <unordered_map>

namespace {
	GLFWwindow* WindowPtr = nullptr;
}

ox::result window_module::init() {
	Logger = dynamic_cast<logger*>(this->Managers[0]);
	Controller = dynamic_cast<controller*>(this->Managers[1]);
	OX_ASSERT(Logger);
	OX_ASSERT(Controller);


	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	OX_ASSERTMSG(glfwInit(),"%s","cannot initialize glfw.");
	WindowPtr = glfwCreateWindow(800, 600, "Omnix Window", nullptr, nullptr);
	OX_ASSERTMSG(WindowPtr, "%s", "cannot create glfw window.");

	glfwMakeContextCurrent(WindowPtr);
	return ox::ok;
};
ox::result window_module::update(double dt) {
	if (glfwWindowShouldClose(WindowPtr)) {
		Controller->Running = false;
	}
	glfwSwapBuffers(WindowPtr);
	return ox::ok;
};
ox::result window_module::shutdown() {
	glfwTerminate();
	glfwDestroyWindow(WindowPtr);
	return ox::ok;
};



namespace {
	std::unordered_map<ox::cstr, KeyEvent> KeyEvents;
	std::unordered_map<ox::cstr, MouseButtonEvent> MouseButtonEvents;
	std::unordered_map<ox::cstr, MouseMoveEvent> MouseMoveEvents;
	std::unordered_map<ox::cstr, MouseScrollEvent> MouseScrollEvents;

	std::unordered_map<ox::cstr, WindowSizeChangeEvent> WindowSizeChangeEvents;


	void _Key_call(GLFWwindow* window, int key, int scancode, int action, int mods){
		for (auto& el : KeyEvents) {
			el.second(key,action);
		}
	}
	void _MouseButton_call(GLFWwindow* window, int button, int action, int mods){
		for (auto& el : MouseButtonEvents) {
			el.second(button,action);
		}
	}
	void _MouseMove_call(GLFWwindow* window, double x,double y){
		for (auto& el : MouseMoveEvents) {
			el.second(x,y);
		}
	}
	void _MouseScroll_call(GLFWwindow* window, /*NOTE: ignore for now.*/double dx,double dy){
		for (auto& el : MouseScrollEvents) {
			el.second(dy);
		}
	}
	void _WindowSizeChange_call(GLFWwindow* window,int w,int h){
		for (auto& el : WindowSizeChangeEvents) {
			el.second(w,h);
		}
	}
}

ox::result window_module::PushKeyEvent(ox::cstr _name,KeyEvent event){
	if(KeyEvents.find(_name)!=KeyEvents.end()) return ox::err;
	KeyEvents[_name] = event;
	glfwSetKeyCallback(WindowPtr, _Key_call);
	return ox::ok;
}
ox::result window_module::PushMouseButtonEvent(ox::cstr _name,MouseButtonEvent event){
    if(MouseButtonEvents.find(_name)!=MouseButtonEvents.end()) return ox::err;
	MouseButtonEvents[_name] = event;
	glfwSetMouseButtonCallback(WindowPtr, _MouseButton_call);
	return ox::ok;
}
ox::result window_module::PushMouseMoveEvent(ox::cstr _name,MouseMoveEvent event){
 	if(MouseMoveEvents.find(_name)!=MouseMoveEvents.end()) return ox::err;
	MouseMoveEvents[_name] = event;
	glfwSetCursorPosCallback(WindowPtr, _MouseMove_call);
	return ox::ok;
}
ox::result window_module::PushMouseScrollEvent(ox::cstr _name,MouseScrollEvent event){
	if(MouseScrollEvents.find(_name)!=MouseScrollEvents.end()) return ox::err;
	MouseScrollEvents[_name] = event;
	glfwSetScrollCallback(WindowPtr, _MouseScroll_call);
	return ox::ok;
}
ox::result window_module::PushWindowSizeChangeEvent(ox::cstr _name,WindowSizeChangeEvent event){
	if(WindowSizeChangeEvents.find(_name)!=WindowSizeChangeEvents.end()) return ox::err;
	WindowSizeChangeEvents[_name] = event;
	glfwSetFramebufferSizeCallback(WindowPtr, _WindowSizeChange_call);
	return ox::ok;
}
	
ox::result window_module::ResizeWindow(ox::vec2i size){
	glfwSetWindowSize(WindowPtr, size.x(), size.y());
	return ox::ok;
}
ox::result window_module::SetTitle(ox::cstr title){
	glfwSetWindowTitle(WindowPtr, title);
	return ox::ok;
}
ox::lvptr window_module::Handle(){
	return WindowPtr;
}
ox::lvptr window_module::ProcAddress() {
	return glfwGetProcAddress;
}