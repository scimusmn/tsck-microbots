#ifndef SMM_STAGE_H
#define SMM_STAGE_H

#include <vector>
#include <chrono>

#include "Stepper.h"
#include "Button.h"

class Stage
{
 public:
    Stage(unsigned int pulX,
          unsigned int dirX,
          unsigned int pulY,
          unsigned int dirY,
          unsigned int stepsPerRevolution,
          unsigned int jsPins[4],
          unsigned int limitX,
          unsigned int limitY,
          double buttonDebounceTime,
          long limits[4],
          double speed, double acceleration);

    void home(double homeSpeed);
    void moveToPosition(double speed, std::vector<long> position);
    
    std::vector<long> getPosition();

    void update();

 private:
    double speed, acceleration;
    double xVel, yVel;
    long minX, minY, maxX, maxY;
    Button jsLeft, jsUp, jsRight, jsDown;
    Button xLimit, yLimit;
    Stepper xAxis, yAxis;

    void updateVelocities(std::chrono::duration<double> delta,
                          int dirX, int dirY);                          

    std::chrono::time_point<std::chrono::steady_clock> prevTime;
};
    
#endif
