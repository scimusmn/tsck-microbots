#ifndef SMM_STAGE_H
#define SMM_STAGE_H

#include <vector>

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
          double speed);

    void home(double homeSpeed);
    
    std::vector<long> getPosition();

    void update();

 private:
    double speed;
    long minX, minY, maxX, maxY;
    Button jsLeft, jsUp, jsRight, jsDown;
    Button xLimit, yLimit;
    Stepper xAxis, yAxis;
};
    
#endif
