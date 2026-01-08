#define OXLIB_DEFAULT_CONFIG
#include <oxlib/oxlib.h>
#include <oxlib/result.h>
#include <oxlib/time.h>
#include <oxlib/math.h>

#include "../modules/window_module.h"
#include "../modules/input_module.h"
#include "../modules/graphics_module.h"
#include "checknull_Module.h"

int main(int argc, char **argv) {
  engine engine;

  game_main game_module(nullptr);
  window_module win_mod(nullptr);
  input_module input_mod(nullptr);
  graphics_module graphics_mod{nullptr};

  engine.add_module(&game_module);
  engine.add_module(&win_mod);
  engine.add_module(&input_mod);
  engine.add_module(&graphics_mod);

  engine.run();
  return 0;
}
