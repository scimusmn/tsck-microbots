#pragma once

#include <Arduino.h>
#include "Motor.h"
#include "CheckError.h"
#include "Position.h"

#define XY_LOG(msg) LOG("[XYTable] ", msg)

#define Y_PUL 3
#define Y_DIR 4
#define Y_ENA 5
#define Y_LIM 9 

#define X_PUL 24
#define X_DIR 23
#define X_ENA 22
#define X_LIM 10


class XYTable {
	protected:
	Stepper xAxis, yAxis;
	long xMax, yMax;

	int _stepsToPixels(long x, long max) {
		return 127 - ((128*x)/max);
	}

	int _pixelsToSteps(long x, long max) {
		return max - ((max*x)/128);
	}

	public:
	XYTable(long xMax, long yMax)
		: xAxis(X_PUL, X_DIR, X_ENA, X_LIM, xMax
#ifdef MIRROR_TABLE
, true), /* invert x-axis */
#else
),       /* do not invert x-axis */
#endif
		  yAxis(Y_PUL, Y_DIR, Y_ENA, Y_LIM, yMax),
		  xMax(xMax), yMax(yMax)
	{}

	void init() {
		XY_LOG("Homing X...");
		xAxis.home();
		XY_LOG("Homing Y...");
		yAxis.home();
	}

	Position getPosition() {
		Position pos;
		pos.x = xAxis.getSteps();
		pos.y = yAxis.getSteps();
		return pos;
	}

	Position toPixels(Position stepPos) {
		Position pixelPos;
		pixelPos.x = _stepsToPixels(stepPos.x, xMax);
#ifdef MIRROR_TABLE
		pixelPos.x = 127 - pixelPos.x;
#endif
		pixelPos.y = _stepsToPixels(stepPos.y, yMax);
		return pixelPos;
	}

	Position toSteps(Position pixelPos) {
		Position stepPos;
		stepPos.x = _pixelsToSteps(pixelPos.x, xMax);
#ifdef MIRROR_TABLE
		stepPos.x = xMax - stepPos.x;
#endif
		stepPos.y = _pixelsToSteps(pixelPos.y, yMax);
		return stepPos;
	}

	void setSpeed(int x, int y) {
		xAxis.setSpeed(x);
		yAxis.setSpeed(y);
	}

	void update() {
		xAxis.update();
		yAxis.update();
	}

	void moveTo(long x, long y, int speed) {
		Position current = getPosition();

		int dx = x - current.x;
		int dy = y - current.y;

		/* determine directions */
		int xSpeed = speed;
		int ySpeed = speed;
		if (dx < 0) xSpeed = -speed;
		if (dy < 0) ySpeed = -speed;

		Serial.print("[XYTable] Start position: (");
		Serial.print(current.x); Serial.print(", "); Serial.print(current.y); Serial.println(")");

		Serial.print("[XYTable] Target position: (");
		Serial.print(x); Serial.print(", "); Serial.print(y); Serial.println(")");

		/* step until distances are equal */
		if (abs(dx) > abs(dy)) {
			/* step x first */
			xAxis.setSpeed(xSpeed);
			XY_LOG("Stepping X first");
			while(abs(dx) != abs(dy)) {
				xAxis.update();
				dx = x - xAxis.getSteps();
			}
		}
		else {
			/* step y first */
			yAxis.setSpeed(ySpeed);
			XY_LOG("Stepping Y first");
			while(abs(dx) != abs(dy)) {
				yAxis.update();
				dy = y - yAxis.getSteps();
			}
		}

		xAxis.setSpeed(xSpeed);
		yAxis.setSpeed(ySpeed);
		XY_LOG("Stepping together");
		while (abs(dx) > 2) {
			xAxis.update(); yAxis.update();
			dx = x - xAxis.getSteps();
			dy = y - yAxis.getSteps();
		}

		xAxis.setSpeed(0);
		yAxis.setSpeed(0);
	}
};
