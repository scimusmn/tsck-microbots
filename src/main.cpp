//#include "ArduinoSerial.h"
//#include "ConnectionManager.h"
//#include <SFML/Graphics.hpp>
//#include <SFML/Audio.hpp>

#include <iostream>
#include <cmath>

extern "C" {
#include <wiringPi.h>
}

#include "Stage.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define JS_LEFT_PIN 23
#define JS_UP_PIN 22
#define JS_RIGHT_PIN 25
#define JS_DOWN_PIN 24

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define STEPPER_PPR 400

#define PUL_X_PIN 30
#define DIR_X_PIN 21

#define PUL_Y_PIN 27
#define DIR_Y_PIN 26

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define LIMIT_X_PIN 0
#define LIMIT_Y_PIN 7

#define MAX_STEPS_X 2000
#define MAX_STEPS_Y 2000

#define BUTTON_DEBOUNCE_TIME 0.1

const double speed = 3;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

double velocityToSpacing(double velocity,
                         unsigned int pulsesPerRevolution);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main()
{
    wiringPiSetup();

    unsigned int jsPins[] = { JS_LEFT_PIN, JS_UP_PIN, JS_RIGHT_PIN, JS_DOWN_PIN };
    long limits[] = { 0, 0, MAX_STEPS_X, MAX_STEPS_Y };
    
    Stage stage(PUL_X_PIN, DIR_X_PIN,
                PUL_Y_PIN, DIR_Y_PIN,
                STEPPER_PPR,
                jsPins,
                LIMIT_X_PIN, LIMIT_Y_PIN,
                BUTTON_DEBOUNCE_TIME,
                limits,
                speed);

    stage.home(3*speed);
    
    while (true) {
        stage.update();
    }
    return 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
