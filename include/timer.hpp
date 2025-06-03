#include <chrono>
#include <functional>
#include <iostream>

using namespace std::chrono;

class Update {
    private:

    uint64_t frame_count = 0;
    time_point<steady_clock> tp_start;
    time_point<steady_clock> tp_update;
    time_point<steady_clock> tp_last;

    duration<double> frame_rate = 0s;

    std::function<void(double)> task;

    public:

        Update(std::function<void(double)> task, double max_fps = 0.0) {

            this->task = task;
            tp_start = steady_clock::now();
            tp_update = tp_start;
            tp_last = tp_start;

            if (max_fps > 0) {
                frame_rate = duration<double>(1.0 / max_fps);
            }
        }

        

        void step() {
            time_point tp_now = steady_clock::now();

            duration<double> elapsed = tp_now - tp_start;

            double DT = duration<double>(tp_now - tp_update).count();
            tp_update = tp_now;
            
            if (tp_now - tp_last >= frame_rate) {

                task(DT);
                
                frame_count ++;
                
                tp_last = tp_now;
                
            }

            if (elapsed >= 1s) {

                const double fps = frame_count / elapsed.count();

                std::cout << "FPS -> " << fps << "\n";

                frame_count = 0;
                tp_start = tp_now;

            }

        }
};