#ifndef DISPLAY__H
#define DISPLAY__H

#include "Common.h"
#include <SDL2/SDL.h>
#include <array>
#include <cstdint>

namespace chip8 {

constexpr uint8_t DISPLAY_SCALE = 10; // Scales to 640x320

class Display {
public:
  // Constructor
  Display();

  // Destructor
  ~Display();

  // TODO: docs
  void render(const std::array<uint8_t, DISPLAY_WIDTH * DISPLAY_HEIGHT>
                  &emuDisplayBuffer);

  // TODO: docs
  void clear();

private:
  // TODO: docs
  SDL_Window *_window = nullptr;

  // TODO: docs
  SDL_Renderer *_renderer = nullptr;

  // TODO: docs
  SDL_Texture *_texture = nullptr;
};

} // namespace chip8

#endif
