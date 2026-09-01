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

  /**
   * Renders the emulator display buffer
   * @param emuDisplayBuffer - The display buffer to render
   */
  void render(const std::array<uint8_t, DISPLAY_WIDTH * DISPLAY_HEIGHT>
                  &emuDisplayBuffer);

  /**
   * Checks SDL2 for events such as:
   * - Rendering to screen
   * - Quiting the application window
   * - Keypad inputs
   *
   * @param keypad - A buffer containing whether a keypad button has been
   * pressed, is updated after reading events
   * @returns the first pressed keypad number in the event queue for the frame
   *
   * NOTE: A sentinal vlaue is returned for unsupported keys or if key press
   * event is unpressed
   */
  uint8_t static checkSdlEvent(KeypadType &keypad);

private:
  /*
   * Decodes an SDL keypress event and updates data structures
   * @param keyCode - The keycode pressed
   * @param keypad - The array of allowed keys (key value is the index)
   * @returns the pressed keys underlying value
   *
   * NOTE: A sentinal value is returned for unsupported keys or if key was
   * unpressed
   */
  uint8_t static decodeSdlKeypress(const Uint32 event,
                                   const SDL_Keycode &keyCode,
                                   KeypadType &keypad);

  // The SDL window
  SDL_Window *_window = nullptr;

  // The SDL renderer
  SDL_Renderer *_renderer = nullptr;

  // The SDL texture
  SDL_Texture *_texture = nullptr;
};

} // namespace chip8

#endif
