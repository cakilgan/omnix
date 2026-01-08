#pragma once
#include <string>
#ifndef INCLUDE_GUARD_OMNIXLIB_ASSET_HEADER_
#define INCLUDE_GUARD_OMNIXLIB_ASSET_HEADER_

#include <oxlib/macros.h>
#include <oxlib/detects.h>
#include <oxlib/types.h>

#ifndef OXLIBCONFIG_SET
OX_INTERNAL_WARNING("config is not set.")
#endif

namespace OX_NAMESPACE{
    struct asset_location{
        std::string location_path;
    };
    struct asset_id{
        std::string asset_name;
        int UniqueID;
    };
    template<typename T>
    struct asset{
        virtual T data() = 0;
        virtual void load() = 0;
        virtual void unload() = 0;
        virtual asset_location location() = 0;
    };
}
#endif