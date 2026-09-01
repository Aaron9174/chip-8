#include "Display.h"
#include "Common.h"

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

  // Rendering accelerated flag uses GPU instead of CPU for drawing
  _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);

  // NOTE: Each RGBA channel is assigned a byte for the SDL_PIXELFORMAT_RGBA8888
  // format
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
  // SDL2 expects 4-byte values per pixel (SDL_PIXELFORMAT_RGBA8888)
  std::array<uint32_t, DISPLAY_WIDTH * DISPLAY_HEIGHT> pixels{};

  for (size_t i = 0; i < emuDisplayBuffer.size(); ++i) {
    // If the pixel is asserted, draw a white 0xFFFFFFFF, otherwise draw black
    pixels[i] = emuDisplayBuffer[i] == 1 ? 0xFFFFFFFF : 0x000000FF;
  }

  // Update texture with the new pixel data
  SDL_UpdateTexture(_texture, nullptr, pixels.data(),
                    DISPLAY_WIDTH * sizeof(uint32_t));

  // Clean the screen, copy the texture, and present it
  SDL_RenderClear(_renderer);
  SDL_RenderCopy(_renderer, _texture, nullptr, nullptr);
  SDL_RenderPresent(_renderer);
}

/****************************************************************************/
/****************************************************************************/
uint8_t Display::checkSdlEvent(KeypadType &keypad) {
  SDL_Event event;
  uint8_t keyInput = KEY_SENTINAL;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      // Exit cleanly
      std::exit(0);
    }

    uint8_t newKeyInput =
        decodeSdlKeypress(event.type, event.key.keysym.sym, keypad);

    // Records the first key stroke
    if (newKeyInput != KEY_SENTINAL && keyInput == KEY_SENTINAL) {
      keyInput = newKeyInput;
    }
  }

  return keyInput;
}

/****************************************************************************/
/****************************************************************************/
uint8_t Display::decodeSdlKeypress(const Uint32 eventType,
                                   const SDL_Keycode &keyCode,
                                   KeypadType &keypad) {

  // Only continue if the event type was a keypress
  if (eventType != SDL_KEYDOWN && eventType != SDL_KEYUP) {
    return KEY_SENTINAL;
  }

  const bool isKeyPressed = (eventType == SDL_KEYDOWN);

  uint8_t keyData = KEY_SENTINAL;
  switch (keyCode) {
  case SDLK_0:
    keyData = 0x0;
    keypad[keyData] = isKeyPressed;
    break;
  case SDLK_1:
    keyData = 0x1;
    keypad[keyData] = isKeyPressed;
    break;
  case SDLK_2:
    keyData = 0x2;
    keypad[keyData] = isKeyPressed;
    break;
  case SDLK_3:
    keyData = 0x3;
    keypad[keyData] = isKeyPressed;
    break;
  case SDLK_4:
    keyData = 0x4;
    keypad[keyData] = isKeyPressed;
    break;
  case SDLK_5:
    keyData = 0x5;
    keypad[keyData] = isKeyPressed;
    break;
  case SDLK_6:
    keyData = 0x6;
    keypad[keyData] = isKeyPressed;
    break;
  case SDLK_7:
    keyData = 0x7;
    keypad[keyData] = isKeyPressed;
    break;
  case SDLK_8:
    keyData = 0x8;
    keypad[keyData] = isKeyPressed;
    break;
  case SDLK_9:
    keyData = 0x9;
    keypad[keyData] = isKeyPressed;
    break;
  case SDLK_a:
    keyData = 0xA;
    keypad[keyData] = isKeyPressed;
    break;
  case SDLK_b:
    keyData = 0xB;
    keypad[keyData] = isKeyPressed;
    break;
  case SDLK_c:
    keyData = 0xC;
    keypad[keyData] = isKeyPressed;
    break;
  case SDLK_d:
    keyData = 0xD;
    keypad[keyData] = isKeyPressed;
    break;
  case SDLK_e:
    keyData = 0xE;
    keypad[keyData] = isKeyPressed;
    break;
  case SDLK_f:
    keyData = 0xF;
    keypad[keyData] = isKeyPressed;
    break;
  }

  if (isKeyPressed) {
    return keyData;
  } else {
    return KEY_SENTINAL;
  }
}

} // namespace chip8
