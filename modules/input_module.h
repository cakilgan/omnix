#pragma once
#include "oxlib/math.h"
#include "oxlib/types.h"
#define OXLIB_DEFAULT_CONFIG
#include <oxlib/oxlib.h>
#include <oxlib/result.h>
#include <oxlib/time.h>
#include "../omnix/common.h"


struct mouse_request{
	enum class type{
		scroll,
		button,
		position
	}Type;
};

struct mouse_button_request:public mouse_request{
	public:
	enum class action{
		up,
		down
	};
	private:
	action Action;
	ox::i32 Button;
	public:
	mouse_button_request(const action& action,ox::i32 button):Action(action),Button(button),mouse_request(mouse_request::type::button){};
	bool Result;

	action GetAction(){return Action;}
	ox::i32 GetButton(){return Button;}
};
struct mouse_scroll_request:public mouse_request{
	public:
	mouse_scroll_request():mouse_request(mouse_request::type::scroll){};
	ox::i8 dy;
};
struct mouse_position_request:public mouse_request{
	public:
	mouse_position_request():mouse_request(mouse_request::type::position){};
	ox::i16 x_pos;
	ox::i16 y_pos;
};

class input_module :public module {
public:
	controller* Controller;
	logger* Logger;
	module_manager* Modules;
	
	

	input_module(manager** Managers) :module(module_info{
		.Static = {
		.UniqueId = 776,
		.Name = "input_module",
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

	ox::vec2s GetMousePos();
	bool IsMouseDown(ox::i8);
	bool IsMouseJustDown(ox::i8);
	double MouseYScroll();

	bool IsKeyJustDown(ox::i32 keycode);
	bool IsKeyDown(ox::i32 keycode);

	ox::vec2f GetMouseDelta();
};