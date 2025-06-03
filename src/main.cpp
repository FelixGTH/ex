#include "SDL2/SDL_mouse.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>

#include <Color.hpp>
#include <WindowConfig.hpp>
#include <EventTriggers.hpp>
#include <DrawInterface.hpp>
#include <Position.hpp>


// Есть 2 реализации:
#include <cstdint>
#include <particles/ParticleStatic.hpp>

#include <iomanip>
#include <clocale>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <memory>


#include <timer.hpp>


class GameLoop;


class EventHandler : public EventTriggers {
public:

  GameLoop & game_loop;
  Position   pointer_pos;
  Position   last_pointer_pos;
  bool       is_left_down  = false;
  bool       is_right_down = false;


  EventHandler( WindowConfig &window_config, GameLoop& game_loop ) 
    : EventTriggers(window_config), game_loop(game_loop) {}


  void on_mouse_motion( Position pos ) override;
  void on_mouse_button_down( Uint8 btn_number ) override;
  void on_mouse_button_up( Uint8 btn_number ) override;
};



class GameLoop { 

public:

  WindowConfig  window_config {
    .window_h = 600,
    .window_w = 900,
    .res_h = 100,
    .res_w = 150,
  };

  // Для примера 2 указателя:
  EventHandler  *event_handler = nullptr; // — "сырой" указатель.
  std::unique_ptr<DrawInterface> drawler; // — "умный" указатель.


  GameLoop() {
    if ( SDL_Init( SDL_INIT_VIDEO ) != 0 ) { // — `SDL_INIT_VIDEO` уже включает `SDL_INIT_EVENTS`.

      throw std::runtime_error( SDL_GetError() );
    }
    
    Particle::init(&window_config);
    
    event_handler = new EventHandler(window_config, *this); // — "сырой" указатель.
    drawler       = std::make_unique<DrawInterface>(window_config);     // — "умный" указатель.
  }


  ~GameLoop() { 

    if ( event_handler ) delete event_handler; //  — динамически выделенная память освобождается вручную.
    // if (drawler)       delete drawler;      //! — эта команда НЕ нужна, так как используется "умный" указатель.
    
    SDL_Quit();
}

  void create_particle(
    Uint8 type,
    Color color
  ) {
    do {
      event_handler->last_pointer_pos.lerp(event_handler->pointer_pos);
      if ( Particle::create_new( event_handler->last_pointer_pos, type ) ) {
        
        drawler->draw_pixel( event_handler->last_pointer_pos, color);
        
      }  
    } while (event_handler->last_pointer_pos.x != event_handler->pointer_pos.x || 
            event_handler->last_pointer_pos.y != event_handler->pointer_pos.y);
  }

  void update(double) { // — максимум кадров в секунду.
    
    // Рисование частиц:
    if ( event_handler->is_left_down ) {

      create_particle(1, { .r = 255 });

    } else if ( event_handler->is_right_down ) {

      create_particle(0, { .b = 255 });

    }

    // Обновление частиц:
    Particle::update_all( *drawler );
  }


  void fixed_update(double) { // — 60 кадров в секунду.
    
    // Торможение частиц:
    Particle::frame_step();
  }


  void run() {

    Update upd([this](double delta_time) {
      update(delta_time);
    });

    Update upd_60([this](double delta_time) {
      fixed_update(delta_time);
    }, 60);

    Update upd_draw([this](double delta_time) {
      drawler->render();
    }, 60);

    while( event_handler->flag_run ) {

      event_handler->handle_events();

      upd.step();
      upd_60.step();
      upd_draw.step();
      

      // SDL_Delay(1); // — снижение нагрузки на CPU.
    }
  }
};



//* Пример рисования:
// void EventHandler::on_mouse_motion(Position pos) {

//   game_loop.drawler->draw_pixel(pos, Color::random());
// }



//* Пример создания частиц:
void EventHandler::on_mouse_motion(Position pos) {
  last_pointer_pos = pointer_pos;

  pointer_pos = pos;
}


void EventHandler::on_mouse_button_down(Uint8 btn_number) {
  
  switch (btn_number) {

    case SDL_BUTTON_LEFT:
      is_left_down  = true;
      break;

    case SDL_BUTTON_RIGHT:
      is_right_down = true;
      break;
  }
}

void EventHandler::on_mouse_button_up(Uint8 btn_number) {

  switch (btn_number) {

    case SDL_BUTTON_LEFT:
      is_left_down  = false;
      break;

    case SDL_BUTTON_RIGHT:
      is_right_down = false;
      break;
  }
}



// SDL2 требует именно такую сигнатуру `main`:
int main( int argc, char *argv[] ) {
      
  setlocale(LC_ALL, "UTF8");
  std::cout << std::setprecision(17) << std::fixed;

  int exit_code = EXIT_SUCCESS;
      
  try {
    
    GameLoop game_loop;
    game_loop.run();
    
      
  } catch (const std::exception& exc) { 
    
    std::cerr << exc.what()     // — описание фактической ошибки, привёдшей в `catch`.
              << "\n" 
              << SDL_GetError() // — сообщение об ошибке, сгенерированное SDL, если есть.
              << "\n"; 

    exit_code = EXIT_FAILURE;
  }

  return exit_code;
}