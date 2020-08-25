#ifndef SMM_STEPPER_H
#define SMM_STEPPER_H

#include <chrono>

class Stepper
{
 public:
    Stepper(unsigned int pulPin,
            unsigned int dirPin,
            unsigned int stepsPerRevolution);

    void setVelocity(double velocity);

    void setSteps(long steps);

    long getSteps();

    void update();
    
 private:
    bool moving, pulState, dirState;
    unsigned int pulPin, dirPin, stepsPerRevolution;
    std::chrono::duration<double> pulseSpacing;
    std::chrono::duration<double> pulseDelta;
    std::chrono::time_point<std::chrono::steady_clock> prevTime;

    long steps;
};
        

#endif
