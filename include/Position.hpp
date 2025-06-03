#include <cstdlib>
#include <string>

#pragma once


struct Position {

  int x = 0;
  int y = 0;
  
  std::string to_string() {
    return "x -> " + std::to_string(x) + " y -> " + std::to_string(y) + "\n";
  }
  

  void lerp(Position other) {

    int delta_x = x - other.x;
    int delta_y = y - other.y;
    
    int abs_delta_x = abs(delta_x);
    int abs_delta_y = abs(delta_y);

    if (delta_x || delta_y) {
      if (abs_delta_x > abs_delta_y) {
        if (delta_x > 0 ) {

          x--;

        } else {
          x++;
        }

      } else if (abs_delta_x < abs_delta_y) {
        if (delta_y > 0 ) {

          y--;

        } else {

          y++;

        }

      } else {
        if (delta_x > 0 ) {

          x--;

        } else {
          x++;
        }
      }
    }
}

};