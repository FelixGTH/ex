#include "Position.hpp"
#include "SDL2/SDL_stdinc.h"
#include <SDL2/SDL_render.h>

#include <WindowConfig.hpp>
#include <Color.hpp>

#include <cstdint>
#include <stdexcept>
#include <vector>


#pragma once


class DrawInterface {

private:

  SDL_Window * window     = nullptr;  // — окно программы.
  SDL_Renderer * renderer = nullptr;  // — рендерер (контекст отрисовки).
  SDL_Texture * texture   = nullptr;  // — текстура для отображения пискелей.

  std::vector<Uint32> pixel_buffer = {};       // — вектор расширяется по размеру текстуры.
  SDL_PixelFormat*    pixel_format = nullptr;  // — структура, хранящая данные о шифровании пикселей.
  Uint32              clear_color;             // — дефолтный цвет фоновых пикселей.

  WindowConfig & window_config;
  

public:

  DrawInterface(WindowConfig & window_config) : window_config(window_config) {

    window = SDL_CreateWindow(
        "Partciles", 
        SDL_WINDOWPOS_CENTERED, // — окно откроется ровно
        SDL_WINDOWPOS_CENTERED, //   в центре экрана.
        
        window_config.window_w, 
        window_config.window_h,

        SDL_WINDOW_SHOWN
    );


    if (window) {
        
        renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
//                                       ^    ^
//          аппаратное ускорение отрисовки    вертикальная синхронизация
        );

    } else throw std::runtime_error("Ошибка при создании `SDL_Window`");


    if (renderer) {
        
        texture = SDL_CreateTexture(
            renderer, 
            SDL_PIXELFORMAT_RGBA8888,    // — 4 цветовых канала по 8 бит каждый.
            SDL_TEXTUREACCESS_STREAMING, // — текстура будет часто изменяться.
            window_config.res_w, 
            window_config.res_h
        );

    } else throw std::runtime_error("Ошибка при создании `SDL_Renderer`");


    if (texture) {

      pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);

    } else throw std::runtime_error("Ошибка при создании `SDL_Texture`");


    if (pixel_format) {
      
      clear_color = SDL_MapRGBA(pixel_format, 0, 0, 0, 255);

      pixel_buffer.resize( window_config.get_res_area(), clear_color );
      
      // Чтобы посмотреть цвета в `SDL_Palette`:
      // SDL_palette_gradient();
      // SDL_palette_step();

    } else throw std::runtime_error("Ошибка при создании `SDL_PixelFormat`");
  }


  ~DrawInterface() {
    
    SDL_FreeFormat      (pixel_format);
    SDL_DestroyTexture  (texture);
    SDL_DestroyRenderer (renderer);
    SDL_DestroyWindow   (window);
    
    // Для проверки работы "умного" указателя:
    // std::cout << "DESTRUCTOR DrawInterface" << std::endl;
  }


  void draw_pixel(Position pos, Color color) {
    
    draw_pixel(
      window_config.pos_to_hash(pos), 
      color.to_sdl_format(pixel_format)
    );
  }


  void draw_pixel(Uint32 pos_hash, Color color) {
    
    draw_pixel(
      pos_hash,
      color.to_sdl_format(pixel_format)
    );
  }


  void draw_pixel(Uint32 pos_hash, Uint32 sdl_color) {
    
    if ( not window_config.pos_in_res(pos_hash) ) return;

    pixel_buffer[ pos_hash ] = sdl_color;
  }


  void clear_pixel(Uint32 pos_hash) {
    
    if ( not window_config.pos_in_res(pos_hash) ) return;

    pixel_buffer[ pos_hash ] = clear_color;
  }


  Uint32 get_pixel(Uint32 pos_hash) {

    return pixel_buffer[pos_hash];
  }


  void SDL_palette_gradient() {

      // Чтобы увидеть, на сколько плавный
      // цветовой переход доступен в `SDL_Palette`:

      for (Uint32 i = 0; i < pixel_buffer.size(); ++i) {

        pixel_buffer[i] = i;
      }

  }

  void SDL_palette_step() {

      // Чтобы увидеть оттенки в `SDL_Palette`:

      for (Uint32 i = 0; i < pixel_buffer.size(); ++i) {

        pixel_buffer[i] = UINT32_MAX / (pixel_buffer.size() * 0.5) * i;  
      }

  }


  void render() {

    // Если бы пришлось очищать std::vector перед каждым обновлением:
    // std::fill(pixel_buffer.begin(), pixel_buffer.end(), clear_color);
    
    // Копирование данных из `std::vector` в данные текстуры:
    SDL_UpdateTexture(texture, nullptr, pixel_buffer.data(), window_config.res_w * sizeof(Uint32));
    //                                                            ^              ^
    //                          указатель на область памяти вектора              количество пикселей в ряду (по ширине) текстуры

    // Копирование текстуры в рендер:
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    
    // Вывод рендера в окно программы:
    SDL_RenderPresent(renderer);
  }
};
