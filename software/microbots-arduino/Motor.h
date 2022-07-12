#pragma once

#include <Arduino.h>
#include "Button.h"
#include "CheckError.h"

#define ST_LOG(msg) LOG("[Stepper] ", msg)

class MagnetMotor {
	public:
	MagnetMotor(int pinA, int pinB) : pinA(pinA), pinB(pinB) {
		pinMode(pinA, OUTPUT);
		pinMode(pinB, OUTPUT);
	}

	stop() {
		digitalWrite(pinA, 0);
		digitalWrite(pinB, 0);
	}

	runLeft() {
		digitalWrite(pinA, 0);
		digitalWrite(pinB, 1);
	}

	void runRight() {
		digitalWrite(pinA, 1);
		digitalWrite(pinB, 0);
	}

	protected:
	int pinA, pinB;
};


class SimpleStepper {
	public:
	SimpleStepper(int pul, int dir, int ena, bool inverted)
		: pul(pul), dir(dir), ena(ena), inverted(inverted), steps(0) {
		pinMode(pul, OUTPUT);
		pinMode(dir, OUTPUT);
		pinMode(ena, OUTPUT);

		digitalWrite(pul, 0);
		setDirection(FORWARD);
		enable();
	}

	void step() {
		if (!enabled) return; // do nothing
		digitalWrite(pul, 1);
		delayMicroseconds(5);
		digitalWrite(pul, 0);
		steps += direction;
	}

	static const int FORWARD = 0;
	static const int BACKWARD = 1;
	void setDirection(int directionConstant) {
		if (inverted)
			digitalWrite(dir, !directionConstant);
		else
			digitalWrite(dir, directionConstant);
		direction = directionConstant == FORWARD ? 1 : -1;
	}

	void enable() {
		enabled = true;
		digitalWrite(ena, 0);
	}

	void disable() {
		enabled = false;
		digitalWrite(ena, 1);
	}

	long getSteps() {
		return steps;
	}

	protected:
	bool enabled, inverted;
	int direction;

	int pul, dir, ena;
	long steps;
};


class LimitSwitch : public Button {
	private:
	bool triggered;

	public:
	LimitSwitch(int pin) : Button(pin) {
		int initialState = digitalRead(pin);
		if (initialState == 0)
			triggered = true;
		else
			triggered = false;
	}

	bool isTriggered() {
		return triggered;
	}

	void onChange(int state) {
		if (state == 0) {
			triggered = true;
		}
		else {
			triggered = false;
		}
	}
};


class Stepper : public SimpleStepper {
	public:
	Stepper(int pul, int dir, int ena, int limit, unsigned long maxSteps, bool inverted=false) 
		: SimpleStepper(pul, dir, ena, inverted), limit(limit), maxSteps(maxSteps) {
		stopped = true;
		lastStepTime = micros();	
	}

	void setSpeed(int stepsPerSecond) {
		if (stepsPerSecond == 0) {
			stopped = true;
		}
		else {
			if (stopped) lastStepTime = micros();
			stopped = false;
		}
		setDirection(stepsPerSecond < 0 ? SimpleStepper::BACKWARD : SimpleStepper::FORWARD);
		stepsPerSecond = abs(stepsPerSecond);
		timePerStep = 1000000/stepsPerSecond;
	}

	void printPrefix() {
		Serial.print("[Stepper 0x");
		Serial.print((int) this, HEX);
		Serial.print("] ");
	}

	void update() {
		if (stopped) return;

		/* account for micros() overflow */
		if ((long long) micros() - lastStepTime < 0) lastStepTime = 0;

		/* prevent multiple steps from happening simultaneously */
		while (micros() - lastStepTime > 2*timePerStep) {
			lastStepTime += timePerStep;
		}

		/* update limit switch */
		limit.update();

/*		if (steps % 100 == 0 && steps != 0) {
			printPrefix();
			Serial.print("Steps: ");
			Serial.println(steps);
		}*/

		/* check if we should step */
		if (limit.isTriggered() && direction == -1) return; // don't do anything else if we're running into the limit switch
		if (steps >= maxSteps && direction == 1) return; // don't step if we'd pass maxSteps
		if ((micros() - lastStepTime) > timePerStep) {
			lastStepTime += timePerStep;
			step();
		}
	}

	void home() {
		/* roughly find home */
		setSpeed(-800);
		while(!limit.isTriggered()) update();
		steps = 0;

		/* move forward a little */
		setSpeed(100);
		while (steps < 300) update();

		/* move back to home */
		setSpeed(-50);
		while(!limit.isTriggered()) update();
		steps = 0;
	}

	protected:
	LimitSwitch limit;
	unsigned long maxSteps;
	unsigned long lastStepTime;
	int timePerStep;
	bool stopped;
};
