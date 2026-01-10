#pragma once
#include "oxlib/macros.h"
#include "oxlib/math.h"
#include "oxlib/types.h"
#define OXLIB_DEFAULT_CONFIG
#include <oxlib/oxlib.h>
#include <oxlib/result.h>
#include <oxlib/time.h>
#include "../omnix/common.h"



struct WindowApi {
	ox::lvptr Handle;
};

OX_TYPEDEF(void(*)(const int key, const int action),KeyEvent);
OX_TYPEDEF(void(*)(const int button, const int action),MouseButtonEvent);
OX_TYPEDEF(void(*)(const ox::i16 xpos, const ox::i16 ypos),MouseMoveEvent);
OX_TYPEDEF(void(*)(ox::i8 delta),MouseScrollEvent);
OX_TYPEDEF(void(*)(ox::i32 width,ox::i32 height), WindowSizeChangeEvent);



class window_module :public module {
	public:
		controller* Controller;
		logger* Logger;
		window_module(manager** Managers) :module(module_info{
			.Static = {
			.UniqueId = 32,
			.Name = "window_module",
			.Version = {0, 1, 0},
			.Type = module_type::STATIC,
			.Dependencies = {
			},
			.Path = nullptr
			}
		},Managers){}

	ox::result init() override;
	ox::result update(double dt) override;
	ox::result shutdown() override;

	ox::lvptr ProcAddress();
	ox::lvptr Handle();
	
	ox::result PushKeyEvent(ox::cstr _name,KeyEvent event);
	ox::result PushMouseButtonEvent(ox::cstr _name,MouseButtonEvent event);
	ox::result PushMouseMoveEvent(ox::cstr _name,MouseMoveEvent event);
	ox::result PushMouseScrollEvent(ox::cstr _name,MouseScrollEvent event);
	ox::result PushWindowSizeChangeEvent(ox::cstr _name,WindowSizeChangeEvent event);
	ox::result ResizeWindow(ox::vec2i size);
	ox::result SetTitle(ox::cstr title);
};