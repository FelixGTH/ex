#include "SDL2/SDL_stdinc.h"

#include "Position.hpp"
#include "DrawInterface.hpp"
#include <WindowConfig.hpp>

#include <iostream>
#include <unordered_map>
#include <unordered_set>


#define DEBUG 0

#if DEBUG
  #include <stdexcept>
#endif

#pragma once


class Particle {

protected:
  
  // Объявление сокращённых типов (2 способа):
  // 1. using:
  using ParticleMap = std::unordered_map<Uint32, Particle>; // — тип карты частиц.
  using MapIterator = ParticleMap::iterator;                // — тип итератора по карте.
  // typedef ParticleMap::iterator MapIterator;
  // 2. typedef:
  typedef void (* Behaviour )(MapIterator&);                // — тип поведения частиц.


  // Поведение частиц 1 вида:

  static void beh_monolit(MapIterator& iterator) {
    /* Ничего не происходит. Статичное поведение. */
  }


  // Поведение частиц 2 вида:

  static void beh_falling(MapIterator& iterator) {
    
    // Замедление обновления частиц:
    
    if ( iterator->second.freq_upd_count > 0 ) {
      return;
    }
    
    iterator->second.freq_upd_count = FREQ_UPD_LIMIT;
    
    //* Падение частиц:
    
    // Нахождение позиции:
    int pos_hash = iterator->first;
    
    Position pos = window_config->hash_to_pos(pos_hash);

    Position new_pos = {
      .x = pos.x,
      .y = pos.y + 1, // — ось y перевёрнута. 
    };
    
    int pos_new_hash = window_config->pos_to_hash(new_pos);

    
    MapIterator other_iterator = _all.find(pos_new_hash);
    
    // Если новая позиция НЕ занята:
    if ( other_iterator == _all.end() ) {
      
      // Отключаем узел карты от карты
      // (ячейка памяти становится самостоятельной):

      auto node = _all.extract(iterator);

      node.key() = pos_new_hash;
      
      auto result = _all.insert(std::move(node));

      if (result.inserted) {
        
        iterator = result.position; // * — идём к только что добавленной сущности.
      }
    }

  } // beh_falling.


  // Свойства частицы:
  Behaviour behaviour  = beh_monolit;
  Uint8 freq_upd_count = FREQ_UPD_LIMIT; // — счётчик обновления.
  Uint32 id = 0;                         // — id для сравнения частиц.


  // Статические поля:
  static ParticleMap _all; // — карта всех частиц.

  inline static WindowConfig * window_config = nullptr;
  
  const inline static Uint8  FREQ_UPD_LIMIT = 1; // — кол-во пропускаемых между обновлениями кадров.
  inline static       Uint32 id_counter     = 0; // — счётчик id.

public:


  // Конструкторы копирования:
  Particle(const Particle&) = delete;
  Particle& operator=(const Particle&) = delete;
  
  // Конструкторы перемещения:
  Particle(Particle&&) = default;
  Particle& operator=(Particle&&) = default;
  
  // Стандартный конструктор:
  Particle() = default;

  // Деструктор:
  ~Particle() {
    // Для отладки:
    std::cout << "PARTICLE DESTROY" << std::endl; 
  }


  static void frame_step() {

    for ( MapIterator it = _all.begin(); it != _all.end(); ++it ) {

      if ( it->second.freq_upd_count ) {

        it->second.freq_upd_count --;
      }
    }
  }

  
  static bool create_new(
    Position pos, 
    Uint8 type = 0
  ) {
    
    #if DEBUG
      if ( not is_inited() ) throw std::runtime_error("`Particle` НЕ инициализирован."); 
    #endif

    int key_pos = window_config->pos_to_hash(pos);
    
    bool is_positive = key_pos > -1;
    
    if ( is_positive ) {

      auto [ it, inserted ] = _all.try_emplace(key_pos);
      
      // Назначение id:
      it->second.id = id_counter++;
      
      // Поведение сущности:
      switch (type) {

        case 1:
          it->second.behaviour = beh_falling;
          break;

        // TODO
      }

      return inserted;
    }

    return is_positive;
  }


  static void update_all( DrawInterface & drawler ) {

    #if DEBUG
      if ( not is_inited() ) throw std::runtime_error("`Particle` НЕ инициализирован."); 
    #endif

    std::unordered_set<Uint32> already_updated;

    Uint32 amount_deleted = 0;
    Uint8  amount_do       = 100;
    
    do {

      amount_do ++;

      for ( MapIterator it = _all.begin(); it != _all.end(); /* инкремент в теле цикла */ ) {
      
        if ( it != _all.end() ) {

          if ( already_updated.contains(it->second.id ) ) {
            
            it++;

          } else {

            already_updated.insert(it->second.id );

            Uint32 start_pos_hash = it->first;
            
            const Uint32 entry_color = drawler.get_pixel(start_pos_hash);

            it->second.behaviour(it);
            
            // Сущность всё ещё на старом месте:
            if (it->first == start_pos_hash) {
              
              // Обновляем итератор, раз он 
              // не был обновлён в функции `behaviour`:
              
              it++;

            } else {

              drawler.clear_pixel(start_pos_hash);

              // Если новая позиция за пределами
              // экрана, частица уничтожается:
              if ( not window_config->pos_in_res(it->first) ) {
                
                it = _all.erase(it); // * — если сущность удалена, получаем итератор следующей.
                
                amount_deleted++;

              } else {

                // Так как сущность переместилась,
                // нужна перерисовка:

                drawler.draw_pixel (it->first, entry_color);
              }
            }
          }
        }
      }
    
    // Ещё не все сущности обновились:
    } while (
      _all.size() + amount_deleted - already_updated.size() > 5
      && amount_do > 0
    );
  }
  

  static bool is_inited() { return window_config != nullptr; }


  static void init(WindowConfig * wind_config) {
    
    #if DEBUG
      if ( not wind_config ) throw std::invalid_argument("Particle :: init :: nullptr WindowConfig");
    #endif

    Particle::window_config = wind_config;

    // _all.reserve(window_config->get_res_area() / 2);
  }
};

std::unordered_map<Uint32, Particle> Particle::_all{};

