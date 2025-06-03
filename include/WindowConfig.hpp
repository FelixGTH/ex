#pragma once

#include "Position.hpp"
#include <iostream>


struct WindowConfig {

  int window_h = 600;
  int window_w = 800;

  int res_h = 300;
  int res_w = 400;


  int get_res_area()    { return res_h    * res_w;    }
  int get_window_area() { return window_h * window_w; }


  bool pos_in_res(int pos_hash) {

    return pos_hash < get_res_area();
  }


  bool pos_in_res(Position& pos) {

    return
      (pos.x >= 0) && (pos.x < res_w) && 
      (pos.y >= 0) && (pos.y < res_h)
    ;
  }


  int pos_to_hash(Position& pos) {

    return pos.y * res_w + pos.x;
  }


  Position hash_to_pos(int hash) {
      
    int x = hash % res_w;
    int y = hash / res_w;

    return {
      .x = x,
      .y = y,
    };
  }
};