//
// Created by cakilgan on 4/2/26.
//

#ifndef OMNIX_SETUP_H
#define OMNIX_SETUP_H
#include <omnix/platform/result.h>

namespace engine {
struct setup_context {};
ox::result_t setup(setup_context *ctx);
} // namespace engine
#endif // OMNIX_SETUP_H
