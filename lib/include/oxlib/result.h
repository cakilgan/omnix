#pragma once
#ifndef INCLUDE_GUARD_OMNIXLIB_RESULT_HEADER_
#define INCLUDE_GUARD_OMNIXLIB_RESULT_HEADER_

#include <oxlib/macros.h>
#include <oxlib/detects.h>
#include <oxlib/types.h>

#ifndef OXLIBCONFIG_SET
OX_INTERNAL_WARNING("config is not set.")
#endif

#define OX_MAKE_RESULT(name,code)\
constexpr const result name = (code);\



OX_NAMESPACEDEF {
	OX_TYPEDEF(i64,result);

	struct result_inf{
		result ref;
		cstr description;
		bool is_error = false;
    };

	OX_MAKE_RESULT(reserved_start, -1000);
	OX_MAKE_RESULT(ok, 1);
	OX_MAKE_RESULT(err, 0);
	OX_MAKE_RESULT(reserved_end, 1000);


#if OX_INLINE_VARS_SUPPORTED
#define OX_RESULT_REGISTRY(Name) inline constexpr result_inf Name[] = {
#define OX_RESULT_REGISTRY_END };
#define OX_REGISTER_RESULT(ref,desc,is_err) {ref,desc,is_err},

	OX_RESULT_REGISTRY(_OX_MainResult_Table)
		OX_REGISTER_RESULT(reserved_start, "reserved result codes start from this value.",false)
		OX_REGISTER_RESULT(ok, "general succes code.", false)
		OX_REGISTER_RESULT(err, "general fail code.", true)
		OX_REGISTER_RESULT(reserved_end, "reserved result codes end on this value.", false)
	OX_RESULT_REGISTRY_END

	constexpr const result_inf* get_result_info(result r) {
		for (auto& it : _OX_MainResult_Table) {
			if (it.ref == r)
				return &it;
		}
		return nullptr;
	}
#endif

}

#endif