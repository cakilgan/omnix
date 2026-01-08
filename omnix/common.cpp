#include "common.h"

ox::result module_manager::load_dynamic_module(ox::cstr path) {
	using CreateModuleFnc = module * (*)(manager**);

	ox::dynamic_lib dll = ox::load_lib(path);
	auto Logger = dynamic_cast<logger*>(this->Managers[0]);
	auto Controller = dynamic_cast<controller*>(this->Managers[1]);
	auto Dependency = dynamic_cast<dependency_manager*>(this->Managers[2]);
	auto Module = dynamic_cast<module_manager*>(this->Managers[3]);

	if (dll.get_handle() == nullptr) {
		Logger->error("dynamic lib handle is null! on path -> %s", path);
		return ox::err;
	}

	if (ox::load_symbol(dll, "CreateModule") == ox::err) {
		Logger->error("Cannot load function %s on path -> %s", "CreateModule", path);
		return ox::err;
	}
	if (ox::load_symbol(dll, "DestroyModule") == ox::err) {
		Logger->error("Cannot load function %s on path -> %s", "DestroyModule", path);
		return ox::err;
	}

	CreateModuleFnc CreateModule = reinterpret_cast<CreateModuleFnc>(dll.symbols[0].Handle);

	OX_ASSERT(CreateModule);

	auto mod = CreateModule(Managers);

	OX_ASSERT(mod);

	mod->get_info_mut().Dynamic.Status = module_status::LOADED;
	mod->fast_access_dynamic = mod->get_info_mut().Dynamic.DynamicModule.mod;

	auto res = mod->init();
	if (res == ox::ok) {
		Logger->info("Initializing " OX_ACOLOR(93,"DYNAMIC_MODULE") " -> %s  = RESULT: %i", mod->get_name(),res);
	}
	else {
		Logger->error("Initializing " OX_ACOLOR(93, "DYNAMIC_MODULE") "%s returned an error code: %i", mod->get_name(),res);
		delete mod;
		ox::free_lib(dll);
		return res;
	}

	mod->get_info_mut().Dynamic.DynamicModule.lib = dll;
	mod->get_info_mut().Dynamic.DynamicModule.mod = mod;

	Modules[mod->get_info().Static.UniqueId] = mod;

	auto shutdown_order = Dependency->resolve_shutdown_order();
	auto loop_order = Dependency->resolve_loop_order();

	Module->DM_LoopOrder = loop_order;
	Module->DM_ShutdownOrder = shutdown_order;
	return ox::ok;
}