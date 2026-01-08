#define OXLIB_DEFAULT_CONFIG
#include <oxlib/oxlib.h>
#include <oxlib/result.h>
#include <oxlib/time.h>
#include <omnix.h>
#include <common.h>

class dll_module :public module {
public:
	dll_module(manager** Managers) :module(module_info{
		.Static = {
		.UniqueId = 40,
		.Name = "dll_module",
		.Version = {0, 1, 0},
		.Type = module_type::DYNAMIC,
		.Dependencies = {
		},
		.Path = nullptr
		}
		}, Managers) {
	}
	controller* Controller = nullptr;
	logger* Logger = nullptr;
	ox::result init() override { 
		Controller = dynamic_cast<controller*>(this->Managers[1]);
		Logger = dynamic_cast<logger*>(this->Managers[0]);
		return ox::ok; 
	}
	ox::result update(double dt) override { 
		return ox::ok; 
	}
	ox::result shutdown() override { 
		return ox::ok; 
	}
};

extern "C" {
	__declspec(dllexport) module* CreateModule(manager** Managers) {
		return new dll_module(Managers);
	}
	__declspec(dllexport) ox::result DestroyModule(module* Module) {
		delete Module;
		return (Module) ? ox::err:ox::ok;
	}
}