#include "SDL2/SDL_stdinc.h"
#include <SDL2/SDL_events.h>

#include <Position.hpp>
#include <WindowConfig.hpp>



#pragma once

class EventTriggers {

public:

  void handle_events() {
    
    SDL_Event event;

    while (SDL_PollEvent(&event)) {

      switch(event.type) {
        
        case SDL_QUIT:

          flag_run = false;

          break;

        case SDL_KEYDOWN:

        
          on_key_down(event.key.keysym.sym);

          break;

        case SDL_KEYUP:

          on_key_up(event.key.keysym.sym);

          break;

        case SDL_MOUSEMOTION:

          on_mouse_motion({
              .x = (event.motion.x * window_config.res_w) / window_config.window_w,
              .y = (event.motion.y * window_config.res_h) / window_config.window_h
          });

          break;

        case SDL_MOUSEBUTTONDOWN:

          on_mouse_button_down(event.button.button);

          break;

        case SDL_MOUSEBUTTONUP:

          on_mouse_button_up(event.button.button);

          break;
      }
    }
    
  }


  WindowConfig& window_config;
  bool flag_run = true;


  EventTriggers(WindowConfig& window_config) 
    : window_config(window_config) {}


  virtual void on_key_down(SDL_Keycode keycode) {}
  virtual void on_key_up  (SDL_Keycode keycode) {}

  virtual void on_mouse_button_down(Uint8 btn_number) {}
  virtual void on_mouse_button_up  (Uint8 btn_number) {}
    
  virtual void on_mouse_motion(Position pos) {}
};
