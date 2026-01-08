#pragma once
#ifndef INCLUDE_GUARD_OX_COMMON_HEADER_
#define INCLUDE_GUARD_OX_COMMON_HEADER_
#include "omnix.h"

#include <oxlib/color.h>
#include <oxlib/lib.h>

#include <unordered_map>
#include <queue>

class logger : public manager {
public:
	template<typename ... Args>
	void log(ox::cstr fmt,Args ...args) {
		printf(fmt, args...);
		printf("\n");
	}
	template<typename ... Args>
	void info(ox::cstr fmt,Args ...args) {
		printf("%s[INFO]:%s ",ox::ansi::green,ox::ansi::reset);
		printf(fmt, args...);
		printf("\n");
	}
	template<typename ... Args>
	void warn(ox::cstr fmt,Args ...args) {
		printf("%s[WARN]:%s ",ox::ansi::yellow,ox::ansi::reset);
		printf(fmt, args...);
		printf("\n");
	}
	template<typename ... Args>
	void debug(ox::cstr fmt, Args ...args) {
		printf("%s[DEBUG]:%s ", ox::ansi::bright_cyan, ox::ansi::reset);
		printf(fmt, args...);
		printf("\n");
	}
	template<typename ... Args>
	void error(ox::cstr fmt,Args ...args) {
		printf("%s[ERROR]:%s ",ox::ansi::red,ox::ansi::reset);
		printf(fmt, args...);
		printf("\n");
	}

	logger(manager** Managers) : manager("logger", Managers) {}

	ox::result init() override {
		return ox::ok;
	};
	ox::result shutdown() override {
		return ox::ok;
	};
};

class controller : public manager {
public:
	bool Running = false;
	float Fps = 0.0f;
	controller(manager** Managers) : manager("logger", Managers) {}

	ox::result init() override {
		Running = true;
		return ox::ok;
	};
	ox::result shutdown() override {
		return ox::ok;
	};

	int step_count = 0;
	float fps_step_accumulator = 0.0f;
	void step(double dt) {
		fps_step_accumulator += static_cast<float>(1.0f/dt);
		step_count++;
	}
	void opt() {
		if (step_count == 50)
		{
			Fps = fps_step_accumulator / static_cast<float>(step_count);
			fps_step_accumulator = 0.0f;
			step_count = 0;
		}
	}
};

class module_manager : public manager {
	std::unordered_map<ox::u64,module*> Modules;
	logger* Logger = nullptr;

	std::vector<ox::u64> InitOrder;
	std::vector<ox::u64> ShutdownOrder;
	std::vector<ox::u64> LoopOrder;

public:
	std::vector<ox::u64> DM_InitOrder;
	std::vector<ox::u64> DM_ShutdownOrder;
	std::vector<ox::u64> DM_LoopOrder;

	module_manager(manager** Managers) : manager("module_manager", Managers) {}
	ox::result init() override {
		Logger = dynamic_cast<logger*>(this->Managers[0]);
		return ox::ok;
	};
	ox::result shutdown() override {
		return ox::ok;
	};

	ox::result load_modules() {
		for (auto& mod_pair : Modules) {
			auto mod = mod_pair.second;
			if (mod->get_info().Static.Type == module_type::STATIC) {
				mod->get_info_mut().Dynamic.Status = module_status::LOADED;
				continue;
			}
			if (!mod->get_info().Static.Path) {
				Logger->error("Module %s has no path.", mod->get_name());
				return ox::err;
			}

			// Dynamic module loading logic would go here

			using CreateModuleFnc = module* (*)(manager**);

			ox::dynamic_lib dll = ox::load_lib(mod->get_info().Static.Path);

			if (dll.get_handle()==nullptr) {
				Logger->error("dynamic lib handle is null! on path -> %s",mod->get_info().Static.Path);
				return ox::err;
			}
			
			if (ox::load_symbol(dll, "CreateModule")==ox::err) {
				Logger->error("Cannot load function %s on path -> %s", "CreateModule", mod->get_info().Static.Path);
				return ox::err;
			}
			if (ox::load_symbol(dll, "DestroyModule") == ox::err) {
				Logger->error("Cannot load function %s on path -> %s", "DestroyModule", mod->get_info().Static.Path);
				return ox::err;
			}

			CreateModuleFnc CreateModule = reinterpret_cast<CreateModuleFnc>(dll.symbols[0].Handle);

			OX_ASSERT(CreateModule);
			
			mod->get_info_mut().Dynamic.DynamicModule =
			{
						CreateModule(Managers),
						dll
			};

			mod->get_info_mut().Dynamic.Status = module_status::LOADED;

			mod->fast_access_dynamic = mod->get_info_mut().Dynamic.DynamicModule.mod;
		}
		return ox::ok;
	}

	void modules_init() {
		for (auto& ordinal : InitOrder) {
			auto var = Modules[ordinal];
			if (var->fast_access_dynamic) {
				var = var->get_info().Dynamic.DynamicModule.mod;
			}

			var->set_managers(this->Managers);
			Logger->info("Initializing module: %s RESULT: %i", var->get_name(),
				var->init());
			var->get_info_mut().Dynamic.Status = module_status::LOADED;
		}
	}
	void modules_update(double dt) {
		for (auto& ordinal : LoopOrder) {
			auto mod = Modules[ordinal];
			if (mod->fast_access_dynamic) {
				mod->fast_access_dynamic->update(dt);
			}else {
				mod->update(dt);
			}
		}
		if (!DM_LoopOrder.empty()) {
			LoopOrder = DM_LoopOrder;
			DM_LoopOrder.clear();
		}
	}
	void modules_shutdown() {
		if (!DM_ShutdownOrder.empty()) {
			ShutdownOrder = DM_ShutdownOrder;
			DM_ShutdownOrder.clear();
		}
		
		for (auto& ordinal : ShutdownOrder) {
			auto var = Modules[ordinal];
			if (var->get_info_mut().Static.Type == module_type::DYNAMIC) {
				using DestroyModuleFnc = ox::result(*)(module*);
				DestroyModuleFnc DestroyModule = reinterpret_cast<DestroyModuleFnc>(var->get_info_mut().Dynamic.DynamicModule.lib.symbols[1].Handle);
				OX_ASSERT(DestroyModule);
				
				Logger->info("Shutting down module: %s RESULT: %i", var->get_info_mut().Dynamic.DynamicModule.mod->get_name(),
					var->get_info_mut().Dynamic.DynamicModule.mod->shutdown());

				auto lib = var->get_info_mut().Dynamic.DynamicModule.lib;
				DestroyModule(var);
				ox::free_lib(lib);
				continue;
			}
			else {
				auto res2 = var->shutdown();
				Logger->info("Shutting down module: %s RESULT: %i", var->get_name(),
					res2);
			}
			var->set_managers(nullptr);
			var->get_info_mut().Dynamic.Status = module_status::UNLOADED;
		}
	}

	void emergency_shutdown() {
		for (auto& mod_pair : Modules) {
			auto var = mod_pair.second;
			Logger->warn("Emergency shutting down module: %s RESULT: %i", var->get_name(),
				var->shutdown());
			
			if (var->get_info_mut().Static.Type == module_type::DYNAMIC) {
				using DestroyModuleFnc = ox::result(*)(module*);
				DestroyModuleFnc DestroyModule = reinterpret_cast<DestroyModuleFnc>(var->get_info_mut().Dynamic.DynamicModule.lib.symbols[1].Handle);
				OX_ASSERT(DestroyModule);
				DestroyModule(var->get_info_mut().Dynamic.DynamicModule.mod);
				ox::free_lib(var->get_info_mut().Dynamic.DynamicModule.lib);
			}
			var->set_managers(nullptr);
			var->get_info_mut().Dynamic.Status = module_status::UNLOADED;
		}
	}

	std::unordered_map<ox::u64, module*>& get_modules(){
		return Modules;
	}

	void set_modules(std::unordered_map<ox::u64, module*> Mods) {
		this->Modules = std::move(Mods);
	}

	void set_init_order(std::vector<ox::u64> Order) {
		this->InitOrder = std::move(Order);
	}
	void set_shutdown_order(std::vector<ox::u64> Order) {
		this->ShutdownOrder = std::move(Order);
	}
	void set_loop_order(std::vector<ox::u64> Order) {
		this->LoopOrder = std::move(Order);
	}
	ox::result load_dynamic_module(ox::cstr path);
};

class dependency_manager : public manager {
	public:
		module_manager* ModuleMgr = nullptr;
		logger* Logger = nullptr;
	dependency_manager(manager** Managers) : manager("dependency_manager", Managers) {}
	ox::result init() override {
		Logger = dynamic_cast<logger*>(this->Managers[0]);
		ModuleMgr = dynamic_cast<module_manager*>(this->Managers[3]);
		return ox::ok;
	};
	ox::result shutdown() override {
		return ox::ok;
	};

	ox::result check_dependencies() {
		const auto& mods = ModuleMgr->get_modules();
		for (auto& mod_ : mods)
		{
			module* mod = mod_.second->fast_access_dynamic;
			if (!mod) {
				mod = mod_.second;
			}

			Logger->debug("[DEPENDENCY CHECK]: %s", mod->get_name());
			if (mod->get_info().Static.Dependencies.empty()) {
				Logger->warn("Module %s has no dependencies.", mod->get_name());
				continue;
			}
			Logger->debug("Module %s has %zu dependencies.", mod->get_name(), mod->get_info().Static.Dependencies.size());

			for (auto& dep : mod->get_info_mut().Static.Dependencies) {
				bool found = false;
				for (auto& check_mod_ : mods) {
					auto check_mod = check_mod_.second;

					if (strcmp(dep.Id, check_mod->get_name()) == 0) {

						if (dep.Version != check_mod->get_info().Static.Version) {
							Logger->error("Dependency %s version mismatch for module %s. Required: %u.%u.%u, Found: %u.%u.%u",
								dep.Id,
								mod->get_name(),
								dep.Version.Major,
								dep.Version.Minor,
								dep.Version.Patch,
								check_mod->get_info().Static.Version.Major,
								check_mod->get_info().Static.Version.Minor,
								check_mod->get_info().Static.Version.Patch);

							if (dep.Type == dependency::dependency_type::REQUIRED)
							{
								Logger->error("Dependency %s is REQUIRED. Failing dependency check for module %s.", dep.Id, mod->get_name());
								dep.State = dependency_state::INVALID_VERSION;
								found = false;
							}
							else {
								Logger->warn("Dependency %s is OPTIONAL. Continuing despite version mismatch for module %s.", dep.Id, mod->get_name());
								dep.State = dependency_state::OPTIONAL_MISSING;
								found = true;
							}
							continue;
						}

						found = true;
						Logger->info("Dependency %s found for module %s.", dep.Id, mod->get_name());
						dep.State = dependency_state::RESOLVED;
						break;
					}
				}
				if (!found) {
					if (dep.Type == dependency::dependency_type::OPTIONAL) {
						Logger->warn("Dependency %s NOT found for module %s, but it is OPTIONAL. Continuing.", dep.Id, mod->get_name());
						dep.State = dependency_state::OPTIONAL_MISSING;
						continue;
					}
					Logger->error("Dependency %s NOT found for module %s.", dep.Id, mod->get_name());
					dep.State = dependency_state::MISSING;
					return ox::err;
				}
			}
		}
		return ox::ok;
	}

	std::vector<ox::u64> resolve_init_order() {
		const auto& mods = ModuleMgr->get_modules();
		using ModuleID = ox::u64;
		std::unordered_map<ModuleID, std::vector<ModuleID>> graph;
		std::unordered_map<ModuleID, int> indegree;

		for (auto& mod_ : mods) {
			auto mod = mod_.second;
			auto id = mod->get_info().Static.UniqueId;
			indegree[id] = 0;
		}

		for (auto& mod_ : mods) {
			auto mod = mod_.second;
			auto from = mod->get_info().Static.UniqueId;

			for (auto& dep : mod->get_info().Static.Dependencies) {
				auto to = dep.UniqueId;

				if(dep.State == dependency_state::OPTIONAL_MISSING){
					continue;
				}

				if (dep.Behavior.init_before) {
					graph[from].push_back(to);
					indegree[to]++;
				}

				if (dep.Behavior.init_after) {
					graph[to].push_back(from);
					indegree[from]++;
				}
			}
		}

		std::queue<ModuleID> q;
		for (auto& [id, deg] : indegree)
			if (deg == 0) q.push(id);

		std::vector<ModuleID> order;

		while (!q.empty()) {
			auto u = q.front(); q.pop();
			order.push_back(u);

			for (auto v : graph[u]) {
				if (--indegree[v] == 0)
					q.push(v);
			}
		}

		if (order.size() != mods.size()) {
			Logger->error("%s","Cyclic dependency detected among modules during initialization order resolution.");
		}
		return order;
	}
	std::vector<ox::u64> resolve_shutdown_order() {
		const auto& mods = ModuleMgr->get_modules();
		using ModuleID = ox::u64;
		std::unordered_map<ModuleID, std::vector<ModuleID>> graph;
		std::unordered_map<ModuleID, int> indegree;

		for (auto& mod_ : mods) {
			auto mod = mod_.second;
			auto id = mod->get_info().Static.UniqueId;
			indegree[id] = 0;
		}

		for (auto& mod_ : mods) {
			auto mod = mod_.second;

			auto from = mod->get_info().Static.UniqueId;

			for (auto& dep : mod->get_info().Static.Dependencies) {
				auto to = dep.UniqueId;
				if (dep.State == dependency_state::OPTIONAL_MISSING) {
					continue;
				}
				if (dep.Behavior.shutdown_before) {
					graph[from].push_back(to);
					indegree[to]++;
				}

				if (dep.Behavior.shutdown_after) {
					graph[to].push_back(from);
					indegree[from]++;
				}
			}
		}

		std::queue<ModuleID> q;
		for (auto& [id, deg] : indegree)
			if (deg == 0) q.push(id);

		std::vector<ModuleID> order;

		while (!q.empty()) {
			auto u = q.front(); q.pop();
			order.push_back(u);

			for (auto v : graph[u]) {
				if (--indegree[v] == 0)
					q.push(v);
			}
		}

		if (order.size() != mods.size()) {
			Logger->error("%s", "Cyclic dependency detected among modules during initialization order resolution.");
		}
		return order;
	}
	std::vector<ox::u64> resolve_loop_order() {
		const auto& mods = ModuleMgr->get_modules();
		using ModuleID = ox::u64;
		std::unordered_map<ModuleID, std::vector<ModuleID>> graph;
		std::unordered_map<ModuleID, int> indegree;

		for (auto& mod_ : mods) {
			auto mod = mod_.second;
			auto id = mod->get_info().Static.UniqueId;
			indegree[id] = 0;
		}

		for (auto& mod_ : mods) {
			auto mod = mod_.second;

			auto from = mod->get_info().Static.UniqueId;

			for (auto& dep : mod->get_info().Static.Dependencies) {
				auto to = dep.UniqueId;
				if (dep.State == dependency_state::OPTIONAL_MISSING) {
					continue;
				}
				if (dep.Behavior.loop_before) {
					graph[from].push_back(to);
					indegree[to]++;
				}

				if (dep.Behavior.loop_after) {
					graph[to].push_back(from);
					indegree[from]++;
				}
			}
		}

		std::queue<ModuleID> q;
		for (auto& [id, deg] : indegree)
			if (deg == 0) q.push(id);

		std::vector<ModuleID> order;

		while (!q.empty()) {
			auto u = q.front(); q.pop();
			order.push_back(u);

			for (auto v : graph[u]) {
				if (--indegree[v] == 0)
					q.push(v);
			}
		}

		if (order.size() != mods.size()) {
			Logger->error("%s", "Cyclic dependency detected among modules during initialization order resolution.");
		}
		return order;
	}
};




#endif