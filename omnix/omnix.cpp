#include "omnix.h"
#include <mutex>

#include "common.h"

#include <oxlib/color.h>
#include <oxlib/lib.h>


#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"


namespace {
    std::mutex _OX_Mutex;
}

void engine::set_run_m() {
  std::scoped_lock lock(_OX_Mutex);
  running = true;
}
void engine::set_stop_m() {
  std::scoped_lock lock(_OX_Mutex);
  running = false;
}
bool engine::get_run_m() {
  std::scoped_lock lock(_OX_Mutex);
  return running;
}

ox::result engine::run() {
  this->Managers = new manager *[16];
  this->Managers[0] = new logger(this->Managers);
  this->Managers[1] = new controller(this->Managers);
  this->Managers[2] = new dependency_manager(this->Managers);
  this->Managers[3] = new module_manager(this->Managers);

  OX_ASSERT(this->Managers);
  OX_ASSERT(this->Managers[0]);
  OX_ASSERT(this->Managers[1]);
  OX_ASSERT(this->Managers[2]);
  OX_ASSERT(this->Managers[3]);

  this->Managers[0]->init();
  this->Managers[1]->init();
  this->Managers[2]->init();
  this->Managers[3]->init();

  auto Logger = dynamic_cast<logger*>(this->Managers[0]);
  auto Controller = dynamic_cast<controller*>(this->Managers[1]);
  auto Dependency = dynamic_cast<dependency_manager*>(this->Managers[2]);
  auto Module = dynamic_cast<module_manager*>(this->Managers[3]);

  std::unordered_map<ox::u64, module*> module_map;
  for (auto& mod : this->Modules) {
    module_map[mod->get_info().Static.UniqueId] = mod;
  }
  Module->set_modules(module_map);


  auto res = Module->load_modules();
  if (res == ox::err) {
      Logger->error("loading modules failed. Exiting.");
      set_stop_m();
      Module->emergency_shutdown();
      delete[] this->Managers;
      return res;
  }


  set_run_m();

  res = Dependency->check_dependencies();
  if (ox::err == res) {
      Logger->error("Dependency check failed. Exiting.");
      set_stop_m();
	  Module->emergency_shutdown();
      delete[] this->Managers;
      return res;
  }

  auto init_order = Dependency->resolve_init_order();
  auto shutdown_order = Dependency->resolve_shutdown_order();
  auto loop_order = Dependency->resolve_loop_order();

  Module->set_init_order(init_order);
  Module->set_shutdown_order(shutdown_order);
  Module->set_loop_order(loop_order);

  Module->modules_init();

  double dt_ms = 0;
  while (get_run_m()) {
    auto start = ox::now();
    //--- main loop start ---

    Module->modules_update(dt_ms);

    if (!Controller->Running) {
		set_stop_m();
    }

    auto elapsed = ox::now() - start;
    dt_ms = (static_cast<float>(elapsed.value()) / 1'000'000'000);
	Controller->step(dt_ms);
    Controller->opt();
  }

  Module->modules_shutdown();

  set_stop_m();

  delete[] this->Managers;
  return ox::ok;
}

