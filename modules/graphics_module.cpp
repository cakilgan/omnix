#include "graphics_module.h"

#include <glad/gl.h>

namespace {
    int glad_ver;
}

ox::result graphics_module::init(){
    Logger = dynamic_cast<logger*>(Managers[0]);
    Controller = dynamic_cast<controller*>(Managers[1]);
    Modules = dynamic_cast<module_manager*>(Managers[3]);
    OX_ASSERT(Logger);
    OX_ASSERT(Controller);
    OX_ASSERT(Modules);
    window = dynamic_cast<window_module*>(Modules->get_modules()[32]);
    OX_ASSERT(window);


    glad_ver = gladLoadGL((GLADloadfunc)window->ProcAddress());
	Logger->debug("GLAD loaded successfully. GL Version: %i.%i",
		GLAD_VERSION_MAJOR(glad_ver),
		GLAD_VERSION_MINOR(glad_ver));
	Logger->debug("INIT PHASE COMPLETED");
    
	glDisable(GL_DEPTH_FUNC);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
		printf("OGL: source=[%i] t=%i id=%i severity=%i : %s\n", source, type, id, severity, std::string(message, length).c_str());
		}, 0);
    return ox::ok;
}
ox::result graphics_module::update(double dt){
	return ox::ok;
}
ox::result graphics_module::shutdown(){
	return ox::ok;
}