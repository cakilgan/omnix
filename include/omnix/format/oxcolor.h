#pragma once
#include <omnix/oxconfig.h>
#include <omnix/oxmacros.h>
#include <omnix/oxdetects.h>
#include <omnix/oxtypes.h>

namespace OX_NAMESPACE {
    enum class Color { RED, GREEN, YELLOW, BLUE, DEFAULT };

    static inline ox::cstr color(Color c) {
        switch(c) {
            case Color::RED:    return "\033[31m";
            case Color::GREEN:  return "\033[32m";
            case Color::YELLOW: return "\033[33m";
            case Color::BLUE:   return "\033[34m";
            default:            return "\033[0m";
        }
    }
}