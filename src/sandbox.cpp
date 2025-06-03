#include <cstdlib>
#include <iostream>

// lerp(1, 5);
// lerp(5, -5);
// lerp(3, 3); - решена

void lerp(int &num1,  int &num2) {
    int delta;


    delta = num1 - num2;

    if (num1 != num2) {
        if (delta > 0) {
            std::cout << "NUM 1 - " << num1 << "\n";
            std::cout << "NUM 2 - " << num2 << "\n";
            num1 ++;
            

        } else {
            std::cout << "NUM 1 - " << num1 << "\n";
            std::cout << "NUM 2 - " << num2 << "\n";
            num2 --;
            

        }
    } else {
        std::cout << num1 << "\n";
        
    }

}

void lerp2(int x1, int y1,  int x2, int y2) {

    int abs_delta_x = std::abs(x1 - x2);
    int abs_delta_y = std::abs(y1 - y2);

    while (x1 != x2 && y1 != y2) {
        std::cout << "ABS_DELTA_X - " << abs_delta_x << " ABS_DELTA_Y - " << abs_delta_y << "\n";
        if (abs_delta_x > abs_delta_y) {
            lerp(x1, x2);

            abs_delta_x--;
            std::cout << abs_delta_x;
        } else if (abs_delta_x < abs_delta_y) {
            lerp(y1, y2);
            abs_delta_y--; 
            std::cout << abs_delta_y;
        } else {
            lerp(y1, y2);
            abs_delta_y--;
            std::cout << abs_delta_y << "\n";
        }
    }
}

int main() {
    lerp2(2, 1, 4, 3);
     
    return 0;
}