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
             double angularVelocity,
             double acceleration)
    : xAxis(pulX, dirX, stepsPerRevolution),
      yAxis(pulY, dirY, stepsPerRevolution),
      jsLeft (jsPins[0], buttonDebounceTime, nullptr),
      jsUp   (jsPins[1], buttonDebounceTime, nullptr),
      jsRight(jsPins[2], buttonDebounceTime, nullptr),
      jsDown (jsPins[3], buttonDebounceTime, nullptr),
      xLimit(limitX, buttonDebounceTime, nullptr),
      yLimit(limitY, buttonDebounceTime, nullptr),
      minX(limits[0]), minY(limits[1]),
      maxX(limits[2]), maxY(limits[3]),
      speed(angularVelocity),
      acceleration(acceleration),
      xVel(0), yVel(0)
{
    prevTime = std::chrono::steady_clock::now();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Stage::home(double homeSpeed)
{
    bool xHome = false;
    bool yHome = false;

    xAxis.setVelocity(-homeSpeed);
    yAxis.setVelocity(-homeSpeed);
    
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

void Stage::moveToPosition(double speed, std::vector<long> position)
{
    bool xArrived = false;
    bool yArrived = false;

    std::vector<long> currentPosition = getPosition();
    
    xAxis.setVelocity( currentPosition[0] < position[0] ? speed : -speed );
    yAxis.setVelocity( currentPosition[1] < position[1] ? speed : -speed );

    while (!(xArrived && yArrived)) {
	xAxis.update(); yAxis.update();
	currentPosition = getPosition();

	if (currentPosition[0] == position[0]) {
	    xArrived = true;
	    xAxis.setVelocity(0);
	}

	if (currentPosition[1] == position[1]) {
	    yArrived = true;
	    yAxis.setVelocity(0);
	}
    }
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
    std::chrono::duration<double> delta = std::chrono::steady_clock::now() - prevTime;
    prevTime = std::chrono::steady_clock::now();
    
    jsLeft.update();
    jsUp.update();
    jsRight.update();
    jsDown.update();

    xLimit.update();
    yLimit.update();

    int dirX = 0;
    int dirY = 0;

    if (!jsLeft.getState() && xAxis.getSteps() < maxX)
        dirX = 1;
    if (!jsUp.getState()   && yAxis.getSteps() < maxY)
        dirY = 1;
    if (!jsRight.getState() && xLimit.getState())
        dirX = -1;
    if (!jsDown.getState() && yLimit.getState())
        dirY = -1;

    updateVelocities(delta, dirX, dirY);
    
    xAxis.setVelocity(xVel);
    yAxis.setVelocity(yVel);

    xAxis.update(); yAxis.update();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Stage::updateVelocities(std::chrono::duration<double> delta,
                             int dirX, int dirY)
{
    double normalization;
    if (dirX == 0 && dirY == 0)
        normalization = 0;
    else
        normalization = 1/sqrt(dirX*dirX + dirY*dirY);
    
    double xVelTarget = speed * dirX * normalization;
    double yVelTarget = speed * dirY * normalization;

    double step = acceleration * delta.count();
    
    int sign = xVelTarget - xVel > 0 ? 1 : -1;
    double diff = xVelTarget - xVel;
    diff = diff > 0 ? diff : -diff;
    if (diff < step)
        xVel = xVelTarget;
    else
        xVel += sign * step;

    sign = yVelTarget - yVel > 0 ? 1 : -1;
    diff = yVelTarget - yVel;
    diff = diff > 0 ? diff : -diff;
    if (diff < step)
        yVel = yVelTarget;
    else
        yVel += sign * step;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
