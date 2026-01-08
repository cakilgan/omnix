#pragma once
#ifndef INCLUDE_GUARD_OMNIX_OMNIX_HEADER_
#define INCLUDE_GUARD_OMNIX_OMNIX_HEADER_
#define OXLIB_DEFAULT_CONFIG
#include <oxlib/oxlib.h>
#include <oxlib/result.h>
#include <oxlib/time.h>
#include <oxlib/lib.h>

#include <vector>

class manager {
protected:
	manager** Managers = nullptr;
	ox::cstr name;
public:
	manager(ox::cstr name,manager** Managers = nullptr):name(name),Managers(Managers){}
	virtual ox::result init() = 0;
	virtual ox::result shutdown() = 0;
	~manager() = default;
};

struct version {
	ox::u32 Major;
	ox::u32 Minor;
	ox::u32 Patch;
	operator bool() const {
		return Major != 0 || Minor != 0 || Patch != 0;
	}
	bool operator==(const version& other) const {
		return Major == other.Major && Minor == other.Minor && Patch == other.Patch;
	}
	bool operator!=(const version& other) const {
		return !(*this == other);
	}
};

enum class dependency_state{
	MISSING,
	INVALID_VERSION,
	RESOLVED,
	OPTIONAL_MISSING
};

struct dependency {
	ox::u64 UniqueId = 0;
	ox::cstr Id;
	version Version;

	enum class dependency_type {
		REQUIRED,
		OPTIONAL
	}Type = dependency_type::REQUIRED;

	struct dependency_behavior {
		bool init_before = false;
		bool init_after = true;
		bool loop_before = false;
		bool loop_after = false;
		bool shutdown_before = false;
		bool shutdown_after = true;
	}Behavior;

	dependency_state State = dependency_state::MISSING;
};

enum class module_type{
	STATIC,
	DYNAMIC
};

enum class module_status{
	UNLOADED,
	LOADED,
	ERROR
};

class module;

struct module_info {
	struct _Static {
		ox::u64 UniqueId;
		ox::cstr Name;
		version Version;
		module_type Type;
		std::vector<dependency> Dependencies{};
		ox::cstr Path = nullptr;
	} Static;
	struct _Dynamic {
		module_status Status = module_status::UNLOADED;
		struct dmodule {
			module* mod = nullptr;
			ox::dynamic_lib lib = nullptr;
		}DynamicModule;
	} Dynamic;
};

class module{
protected:
	manager** Managers = nullptr;
	module_info Info;
public:
	module(const module_info& info,manager** Managers = nullptr):Info(info),Managers(Managers){}
	virtual ox::result init() = 0;
	virtual ox::result update(double dt) = 0;
	virtual ox::result shutdown() = 0;

	//only change nullptr when you use dlls.
	module* fast_access_dynamic = nullptr;

	ox::cstr get_name() const{
		return this->Info.Static.Name;
	}
	const module_info& get_info(){
		return this->Info;
	}
	module_info& get_info_mut(){
		return this->Info;
	}
	
	void set_managers(manager** Managers){
		this->Managers = Managers;
	}
	virtual ~module() = default;
};

class engine 
{
	manager** Managers = nullptr;
	std::vector<module*> Modules;
	bool running = false;
	void set_run_m();
	void set_stop_m();
	bool get_run_m();
public:
	ox::result add_module(module* mod) {
		Modules.push_back(mod);
		return ox::ok;
	}
	ox::result run();
};

#endif