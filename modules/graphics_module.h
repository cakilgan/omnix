#pragma once
#define OXLIB_DEFAULT_CONFIG
#include <oxlib/oxlib.h>
#include <oxlib/result.h>
#include <oxlib/time.h>
#include "../omnix/common.h"


#include "../modules/window_module.h"


class graphics_module :public module {
    private:
public:
	controller* Controller;
	logger* Logger;
	module_manager* Modules;
    window_module* window;
	graphics_module(manager** Managers) :module(module_info{
		.Static = {
		.UniqueId = 1000,
		.Name = "graphics_module",
		.Version = {0, 1, 0},
		.Type = module_type::STATIC,
		.Dependencies = {
			dependency{
                32,"window_module",{0,1,0},
                dependency::dependency_type::REQUIRED,
                {
                    .init_before = false,
                    .init_after = true,
                    .loop_before = true,
                    .loop_after = false,
                    .shutdown_before = true,
                    .shutdown_after = false,
                }
            }
		},
		.Path = nullptr
		}
		}, Managers) {
	}
	ox::result init() override;
	ox::result update(double dt) override;
	ox::result shutdown() override;
};