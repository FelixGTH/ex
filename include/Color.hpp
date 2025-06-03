#include <SDL2/SDL_render.h>
#include <cstdlib>

#pragma once

struct Color {

  Uint8 r = 0;
  Uint8 g = 0;
  Uint8 b = 0;
  Uint8 a = 255;



  Uint32 to_sdl_format(SDL_PixelFormat *format) {
  //                                    ^
  //                                    формат определяет кодировку цвета в 8, 16 или 32 бит.

    return SDL_MapRGBA(format, r, g, b, a);
    //     ^
    //     возвращает индекс цвета, наиболее близкого
    //     к указанному значению в палитре SDL.
  }

  static Color random() {
    return Color{
      .r = static_cast<Uint8>(std::rand() % 255),
      .g = static_cast<Uint8>(std::rand() % 255),
      .b = static_cast<Uint8>(std::rand() % 255),
    };
  }
};