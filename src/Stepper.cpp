#include "Stepper.h"
#include <wiringPi.h>
#include <iostream>

using namespace std::chrono;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Stepper::Stepper(unsigned int pulPin,
                 unsigned int dirPin,
                 unsigned int stepsPerRevolution)
    : pulPin(pulPin), dirPin(dirPin),
      stepsPerRevolution(stepsPerRevolution),
      moving(false), pulState(false), dirState(false),
      pulseSpacing(0), pulseDelta(0),
      prevTime(steady_clock::now())
{
    pinMode(pulPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    digitalWrite(pulPin, false);
    digitalWrite(dirPin, false);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Stepper::setVelocity(double velocity)
{
    if (velocity == 0) {
        moving = false;
    }
    else {
        moving = true;
        // half time so that one full pulse cycle will
        // occur in the total time
        double spacing = 3.141592/(velocity * stepsPerRevolution);
        spacing = spacing > 0 ? spacing : -spacing;
        pulseSpacing = duration<double>(spacing);
        dirState = velocity > 0;
        digitalWrite(dirPin, dirState);
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Stepper::setSteps(long steps)
{
    this->steps = steps;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

long Stepper::getSteps()
{
    return steps;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Stepper::update()
{
    duration<double> delta = steady_clock::now() - prevTime;
    prevTime = steady_clock::now();

    if (!moving) {
        return;
    }
    
    pulseDelta += delta;
    if (pulseDelta > pulseSpacing) {
        pulState = !pulState;
        digitalWrite(pulPin, pulState);
        if (pulState) {
            int sign = dirState ? -1 : 1;
            steps += sign;
        }
        pulseDelta -= pulseSpacing;
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
