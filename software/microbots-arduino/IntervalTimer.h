#pragma once

#include <Arduino.h>

class IntervalTimer {
	public:
	IntervalTimer(unsigned long milliseconds) {
		interval = milliseconds;
	}

	void start() {
		startTime = millis();
	}

	bool triggered() {
		if (millis() - startTime > interval) {
			startTime += interval;
			return true;
		}
		return false;
	}

	protected:
		unsigned long startTime, interval;
};
