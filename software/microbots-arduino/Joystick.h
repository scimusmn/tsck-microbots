#pragma once

#include "Button.h"

class Joystick;

class JoystickButton : public Button {
	public:
	JoystickButton(int pin, int direction, Joystick& js) : Button(pin), direction(direction), js(js) {}

	void onChange(int state);

	protected:
	Joystick& js;
	int direction;
};


class Joystick {
	public:
	friend class JoystickButton;
	static const int UP    = 0b0001;
	static const int RIGHT = 0b0010;
	static const int DOWN  = 0b0100;
	static const int LEFT  = 0b1000;

	Joystick(int up, int right, int down, int left)
		: up(up, UP, *this), right(right, RIGHT, *this),
		  down(down, DOWN, *this), left(left, LEFT, *this),
		  direction(0), changed(false)
	{}

	void directionUpdate() {
		changed = true;
	}

	void update() {
		up.update();
        right.update();
        down.update();
		left.update();
	}

	int getDirection() { 
		changed = false;
		return direction;
	}
	bool hasChanged() { return changed; }
	
	protected:
	JoystickButton up, right, down, left;
	int direction;
	bool changed;
};


void JoystickButton::onChange(int state) {
	if (state == 0) // press
		js.direction |= direction;
	else // release
		js.direction &= ~direction;
	js.directionUpdate();
}

