#include <SFML/Graphics.hpp>
//#include <SFML/Audio.hpp>

#include <iostream>
#include <cmath>

extern "C" {
#include <wiringPi.h>
}

#include "Stage.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define JS_LEFT_PIN 23
#define JS_UP_PIN 24
#define JS_RIGHT_PIN 25
#define JS_DOWN_PIN 22

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define STEPPER_SPR 400  // steps per revolution
#define STEPPER_SPEED 3  // radians per second
#define STEPPER_ACCEL 15 // radians per second per second

#define PUL_X_PIN 30
#define DIR_X_PIN 21

#define PUL_Y_PIN 27
#define DIR_Y_PIN 26

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define LIMIT_X_PIN 0
#define LIMIT_Y_PIN 7

#define MAX_STEPS_X 16000
#define MAX_STEPS_Y 16000

#define BUTTON_DEBOUNCE_TIME 0.1

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool positionIsOnPath(sf::Image& path, Stage& stage);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main()
{
    sf::Image path;
    if (!path.loadFromFile("path.jpg")) {
        std::cerr << "ERROR: failed to load path image!" << std::endl;
        return 1;
    }

    wiringPiSetup();

    unsigned int jsPins[] = { JS_LEFT_PIN, JS_UP_PIN, JS_RIGHT_PIN, JS_DOWN_PIN };
    long limits[] = { 0, 0, MAX_STEPS_X, MAX_STEPS_Y };
    
    Stage stage(PUL_X_PIN, DIR_X_PIN,
                PUL_Y_PIN, DIR_Y_PIN,
                STEPPER_SPR,
                jsPins,
                LIMIT_X_PIN, LIMIT_Y_PIN,
                BUTTON_DEBOUNCE_TIME,
                limits,
                STEPPER_SPEED, STEPPER_ACCEL);

    stage.home(3*STEPPER_SPEED);

    bool onPath = false;
    
    while (true) {
        stage.update();
        if (positionIsOnPath(path, stage) && !onPath) {
            onPath = true;
            std::cout << "on path" << std::endl;
        }
        else if (!positionIsOnPath(path, stage) && onPath) {
            onPath = false;
            std::cout << "off path" << std::endl;
        }
    }
    return 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool positionIsOnPath(sf::Image& path, Stage& stage)
{
    sf::Vector2u size = path.getSize();
    
    std::vector<long> stagePosition = stage.getPosition();
    
    double fractionX = double(stagePosition[0]) / MAX_STEPS_X;
    double fractionY = double(stagePosition[1]) / MAX_STEPS_Y;

    // clamp to [0,1]
    fractionX = fractionX < 0 ? 0 : fractionX;
    fractionX = fractionX > 1 ? 1 : fractionX;

    fractionY = fractionY < 0 ? 0 : fractionY;
    fractionY = fractionY > 1 ? 1 : fractionY;

    unsigned int pixelX = fractionX * size.x;
    unsigned int pixelY = fractionY * size.y;

    sf::Color pixel = path.getPixel(pixelX, pixelY);

    //std::cout << "(" << fractionX << ", " << fractionY << "): "
    //          << (int) pixel.r << " "
    //          << (int) pixel.g << " "
    //          << (int) pixel.b << std::endl;
    
    if ( pixel.r < 128 &&
         pixel.g < 128 &&
         pixel.b < 128 )
        return true;
    else
        return false;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        
