#include <iostream>
#include "../include/Position.hpp"


bool test(int X_1, int Y_1, int X_2, int Y_2, int STEP_COUNT) {
    
    int step_count = STEP_COUNT;

    Position pos1 {
        .x = X_1,
        .y = Y_1
    };

    Position pos2 {
        .x = X_2,
        .y = Y_2
    };

    do {
        pos1.lerp(pos2);
        step_count--;
        // if (step_count < 0) {
        //     break;
        // }

    } while (pos1.x != pos2.x || pos1.y != pos2.y);

    return step_count == 0;
}

#define TEST(X_1, Y_1, X_2, Y_2, STEP_COUNT) {\
int step_count = test(X_1, Y_1, X_2, Y_2, STEP_COUNT);\
std::cout << "---------------------------\n";\
if (step_count == 0) std::cout << #X_1 " " #Y_1 " " #X_2 " " #Y_2 "\nsuccess" << std::endl;\
else std::cerr << #X_1 " " #Y_1 " " #X_2 " " #Y_2 "\nerror step_count -> " << step_count << std::endl;}

int main() {

    TEST(-1, -2, 1, 2, 5); // ok
    TEST(1, 2, -1, -2, 5); // ok
    TEST(1, 1, 3, 1, 3);  // ok
    TEST(3, 1, 1, 1, 5); // ok
    TEST(1, 1, 3, 1, 5); // ok
    TEST(3, 1, 1, 1, 5); // ok
    TEST(-2, 0, 4, 0, 7); // ok 
    TEST(4, 0, -2, 0, 7); // ok
    TEST(-2, -1, -2, -1, 1); // ok
    TEST(4, 1, 4, 1, 1); // ok 

    TEST(2, -2, -3, 4, 12); // ok

    return 0;
}