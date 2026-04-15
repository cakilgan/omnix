#ifndef OMNIX_ARGS_H
#define OMNIX_ARGS_H
#include <iostream>
#include <omnix/core/engine_memory_internal.h>
#include <omnix/core/strview.h>
#include <omnix/core/vector.h>
namespace engine {
struct args {
  private:
    char **_raw;
    int _element_size;

  public:
    args(char **raw, int element_size)
        : _raw(raw), _element_size(element_size) {}
};
} // namespace engine

#endif // ARGS_H
