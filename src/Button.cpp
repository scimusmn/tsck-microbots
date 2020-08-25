#include "Button.h"

Button::Button(int pin, double debounceTime, void (*callback)(bool))
    : pin(pin), debounceTime(debounceTime), state(false), debouncing(false), callback(callback)
{
    pinMode(pin, INPUT);
}

bool Button::getState() { return state; }

void Button::update()
{
    if (debouncing &&
	debounceTime < (std::chrono::steady_clock::now() - debounceStartTime))
	debouncing = false;
    
    if (digitalRead(pin) != state &&
	     !debouncing) {
	debouncing = true;
	state = !state;
	debounceStartTime = std::chrono::steady_clock::now();
	if (callback != nullptr)
	    callback(state);
    }
}
	
