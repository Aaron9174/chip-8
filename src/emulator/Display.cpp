#include "Display.h"

namespace chip8 {

/****************************************************************************/
/****************************************************************************/
Display::Display() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    throw std::runtime_error("[Error] Display error, could not initialize SDL");
  }

  _window =
      SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, DISPLAY_WIDTH * DISPLAY_SCALE,
                       DISPLAY_HEIGHT * DISPLAY_SCALE, SDL_WINDOW_SHOWN);

  _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);

  _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888,
                               SDL_TEXTUREACCESS_STREAMING, DISPLAY_WIDTH,
                               DISPLAY_HEIGHT);
}

/****************************************************************************/
/****************************************************************************/
Display::~Display() {
  SDL_DestroyTexture(_texture);
  SDL_DestroyRenderer(_renderer);
  SDL_DestroyWindow(_window);
  SDL_Quit();
}

/****************************************************************************/
/****************************************************************************/
void Display::render(const std::array<uint8_t, DISPLAY_WIDTH * DISPLAY_HEIGHT>
                         &emuDisplayBuffer) {
  std::array<uint32_t, DISPLAY_WIDTH * DISPLAY_HEIGHT> pixels{};

  for (size_t i = 0; i < emuDisplayBuffer.size(); ++i) {
    // If the pixel is asserted, draw a white 0xFFFFFFFF, otherwise draw black
    pixels[i] = emuDisplayBuffer[i] == 1 ? 0xFFFFFFFF : 0x000000FF;
  }

  // Update texture with the new pixel data
  SDL_UpdateTexture(_texture, nullptr, pixels.data(),
                    DISPLAY_WIDTH * sizeof(uint32_t));

  // Clearn the screen and copy the texture
  SDL_RenderClear(_renderer);
  SDL_RenderCopy(_renderer, _texture, nullptr, nullptr);
  SDL_RenderPresent(_renderer);
}

/****************************************************************************/
/****************************************************************************/
void Display::clear() { SDL_RenderClear(_renderer); }

} // namespace chip8
