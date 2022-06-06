#pragma once

#include <Arduino.h>
#include "Motor.h"
#include "CheckError.h"

#define XY_LOG(msg) LOG("[XYTable] ", msg)

#define Y_PUL 2
#define Y_DIR 3
#define Y_ENA 20
#define Y_LIM 7 

#define X_PUL 24
#define X_DIR 23
#define X_ENA 22
#define X_LIM 8


class XYTable {
	protected:
	Stepper xAxis, yAxis;

	public:
	XYTable()
		: xAxis(X_PUL, X_DIR, X_ENA, X_LIM),
		  yAxis(Y_PUL, Y_DIR, Y_ENA, Y_LIM)
	{}

	void init() {
		XY_LOG("Homing X...");
		xAxis.home();
		XY_LOG("Homing Y...");
		yAxis.home();
	}

	void getPosition(long *x, long *y) {
		*x = xAxis.getSteps();
		*y = yAxis.getSteps();
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
		long currentX, currentY;
		getPosition(&currentX, &currentY);

		int dx = x - currentX;
		int dy = y - currentY;

		/* determine directions */
		int xSpeed = speed;
		int ySpeed = speed;
		if (dx < 0) xSpeed = -speed;
		if (dy < 0) ySpeed = -speed;

		Serial.print("[XYTable] Start position: (");
		Serial.print(currentX); Serial.print(", "); Serial.print(currentY); Serial.println(")");

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
		while (abs(dx) > 10) {
			xAxis.update(); yAxis.update();
			dx = x - xAxis.getSteps();
			dy = y - yAxis.getSteps();
		}

		xAxis.setSpeed(0);
		yAxis.setSpeed(0);
	}
};
