#pragma once
#ifndef INCLUDE_GUARD_OMNIXLIB_COLOR_HEADER_
#define INCLUDE_GUARD_OMNIXLIB_COLOR_HEADER_
#include <oxlib/oxlib.h>
#define OX_ACOLOR(x,v) "\033[" #x "m" v "\033[0m"
OX_NAMESPACEDEF {
	namespace ansi {
		constexpr ox::cstr reset = "\033[0m";
		constexpr ox::cstr black = "\033[30m";
		constexpr ox::cstr red = "\033[31m";
		constexpr ox::cstr green = "\033[32m";
		constexpr ox::cstr yellow = "\033[33m";
		constexpr ox::cstr blue = "\033[34m";
		constexpr ox::cstr magenta = "\033[35m";
		constexpr ox::cstr cyan = "\033[36m";
		constexpr ox::cstr white = "\033[37m";
		constexpr ox::cstr bright_black = "\033[90m";
		constexpr ox::cstr bright_red = "\033[91m";
		constexpr ox::cstr bright_green = "\033[92m";
		constexpr ox::cstr bright_yellow = "\033[93m";
		constexpr ox::cstr bright_blue = "\033[94m";
		constexpr ox::cstr bright_magenta = "\033[95m";
		constexpr ox::cstr bright_cyan = "\033[96m";
		constexpr ox::cstr bright_white = "\033[97m";
	}
}
#endif