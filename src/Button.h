#ifndef SMM_BUTTON_H
#define SMM_BUTTON_H

#include <chrono>
#include <wiringPi.h>

class Button
{
 public:
    Button(int pin, double debounceTime, void (*callback)(bool) = nullptr);

    bool getState();
    void update();

 private:
    int pin;
    bool state, debouncing;
    std::chrono::duration<double> debounceTime;
    std::chrono::steady_clock::time_point debounceStartTime;
    void (*callback)(bool);
};

#endif
