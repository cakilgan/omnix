//
// Created by cakilgan on 4/2/26.
//

#include <iostream>
#include <omnix/omnix.h>
#include <omnix/runtime/setup.h>

#include "omnix/runtime/logger.h"
struct player_pos {
    float xpos,ypos;
};

using engine::literals::operator ""_k;

ox::result_t engine::setup(engine::setup_context* cx) {
    return ox::ok;
}
