#include "Stage.h"
#include <cmath>
#include <iostream>

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Stage::Stage(unsigned int pulX,
             unsigned int dirX,
             unsigned int pulY,
             unsigned int dirY,
             unsigned int stepsPerRevolution,
             unsigned int jsPins[4],
             unsigned int limitX,
             unsigned int limitY,
             double buttonDebounceTime,
             long limits[4],
             double angularVelocity)
    : xAxis(pulX, dirX, stepsPerRevolution),
      yAxis(pulY, dirY, stepsPerRevolution),
      jsLeft (jsPins[0], buttonDebounceTime, nullptr),
      jsUp   (jsPins[1], buttonDebounceTime, nullptr),
      jsDown (jsPins[2], buttonDebounceTime, nullptr),
      jsRight(jsPins[3], buttonDebounceTime, nullptr),
      xLimit(limitX, buttonDebounceTime, nullptr),
      yLimit(limitY, buttonDebounceTime, nullptr),
      minX(limits[0]), minY(limits[1]),
      maxX(limits[2]), maxY(limits[3]),
      speed(angularVelocity)
{}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Stage::home(double homeSpeed)
{
    bool xHome = false;
    bool yHome = false;

    xAxis.setVelocity(homeSpeed);
    yAxis.setVelocity(homeSpeed);
    
    while(!(xHome && yHome)) {
        xLimit.update(); yLimit.update();
        xAxis.update();  yAxis.update();

        if (!xLimit.getState()) {
            xHome = true;
            xAxis.setVelocity(0);
        }

        if (!yLimit.getState()) {
            yHome = true;
            yAxis.setVelocity(0);
        }
    }

    xAxis.setSteps(0);
    yAxis.setSteps(0);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

std::vector<long> Stage::getPosition()
{
    std::vector<long> pos;
    pos.push_back(xAxis.getSteps());
    pos.push_back(yAxis.getSteps());
    return pos;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Stage::update()
{
    jsLeft.update();
    jsUp.update();
    jsRight.update();
    jsDown.update();

    xLimit.update();
    yLimit.update();

    int dirX = 0;
    int dirY = 0;

    if (!jsLeft.getState())
        dirX = -1;
    if (!jsUp.getState())
        dirY = 1;
    if (!jsRight.getState())
        dirX = 1;
    if (!jsDown.getState())
        dirY = -1;

    double normalization = 1/sqrt(dirX*dirX + dirY*dirY);
    double xVel = speed * dirX * normalization;
    double yVel = speed * dirY * normalization;

    std::cout << xVel << ", " << yVel << std::endl;
        
    xAxis.setVelocity(xVel);
    yAxis.setVelocity(yVel);

    xAxis.update(); yAxis.update();
}
