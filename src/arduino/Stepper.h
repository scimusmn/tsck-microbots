#ifndef STEPPER_H
#define STEPPER_H

#define PI 3.1415926

class Stepper
{
 public:
    Stepper() {}
    
    void setup(unsigned int pulse_pin,
               unsigned int dir_pin,
               unsigned int pulsesPerRevolution)
    {
        pul = pulse_pin;
        dir = dir_pin;
        pinMode(pul, OUTPUT);
        pinMode(dir, OUTPUT);
        digitalWrite(pul, 0);
        digitalWrite(dir, 0);
        pulState = false;
        dirState = false;
        this->pulsesPerRevolution = pulsesPerRevolution;
        prevTime = millis();
        setVelocity(0);
        delta = 0;
        phi = 0;
        steps = 0;
    }

    void setVelocity(float angularVelocity)
    {
        phi_dot = angularVelocity;
        moving = true;
        if (phi_dot == 0)
            moving = false;
        else if (phi_dot < 0) {
            digitalWrite(dir, 1);
            dirState = true;
        }
        else {
            digitalWrite(dir, 0);
            dirState = false;
        }
        updatePulseSpacing();
    }

    void step()
    {
        digitalWrite(pul, !pulState);
        pulState = !pulState;
        int sign = !dirState ? 1 : -1;
        steps += sign;
        phi += sign * 2*PI/pulsesPerRevolution;
    }

    void update()
    {
        unsigned int dt = millis() - prevTime;

        if (moving) {
            delta += dt;
            if (delta > pulseSpacing) {
                delta -= pulseSpacing;
                step();
            }
        }
        prevTime = millis();
    }

private:
    unsigned int pul, dir;
    bool pulState, dirState, moving;
    unsigned int pulsesPerRevolution, pulseSpacing;
    unsigned long prevTime, delta;
    float phi, phi_dot;
    unsigned int steps;
    
    void updatePulseSpacing()
    {
        if (moving)
            pulseSpacing = 1000 * PI / (abs(phi_dot) * pulsesPerRevolution);
    }
};
    
        

#endif
