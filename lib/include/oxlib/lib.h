#pragma once
#ifndef INCLUDE_GUARD_OXLIB_LIB_HEADER_
#define INCLUDE_GUARD_OXLIB_LIB_HEADER_

#include <oxlib/macros.h>
#include <oxlib/detects.h>
#include <oxlib/types.h>
#include <oxlib/result.h>
#include <vector>

#ifndef OXLIBCONFIG_SET
OX_INTERNAL_WARNING("config is not set.")
#endif

OX_NAMESPACEDEF {
	struct dynamic_lib {
	private:
		ox::lvptr Handle = nullptr;
	public:
		
		struct symbol{
			ox::cstr name;
			ox::lvptr Handle;
		};

		dynamic_lib(ox::lvptr _Ptr):Handle(_Ptr){}
		ox::cstr path;
		std::vector<symbol> symbols;

		ox::lvptr get_handle() {
			return Handle;
		}
	};
	dynamic_lib load_lib(ox::cstr lib_path);
	ox::result load_symbol(dynamic_lib& lib,ox::cstr Name);
	ox::result free_lib(dynamic_lib& lib);
}


#endif