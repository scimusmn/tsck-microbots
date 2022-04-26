#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

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
#define STEPPER_SPEED 9  // radians per second
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

bool findPosition(sf::Image& path, sf::Color color, sf::Vector2u& position);
sf::Vector2u getPosition(sf::Image& path, Stage& stage);
bool positionIsOnPath(sf::Image& path, sf::Vector2u position);
std::vector<long> pathPosToStage(sf::Image& path, sf::Vector2u position);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main()
{
    sf::Image path;
    if (!path.loadFromFile("path.png")) {
        std::cerr << "ERROR: failed to load path image!" << std::endl;
        return 1;
    }

    sf::SoundBuffer offPathBuffer;
    if (!offPathBuffer.loadFromFile("off-path.wav")) {
	std::cerr << "ERROR: failed to load off-path sound!" << std::endl;
	return 2;
    }
    sf::Sound offPathSound(offPathBuffer);
    offPathSound.setLoop(true);

    sf::Vector2u startPosition;
    sf::Vector2u endPosition;
    sf::Color startColor(255,0,0);
    sf::Color endColor(0,0,255);
        
    if (!findPosition(path, startColor, startPosition)) {
	std::cerr << "WARNING: no start position found; defaulting to (0,0)" << std::endl;
	startPosition.x = 0;
	startPosition.y = 0;
    }

    if (!findPosition(path, endColor, endPosition)) {
	std::cerr << "WARNING: no end position found; defaulting to (" <<
	    endPosition.x << ", " << endPosition.y << ")" << std::endl;
    }

    std::cout << "Start: ("
	      << startPosition.x << ", " << startPosition.y
	      << ")" << std::endl;
    std::cout << "End: ("
	      << endPosition.x << ", " << endPosition.y
	      << ")" << std::endl;

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

    stage.home(STEPPER_SPEED);

    bool onPath = false;

    std::vector<long> startStagePos = pathPosToStage(path, startPosition);
    stage.moveToPosition(STEPPER_SPEED, startStagePos);
    
    while (true) {
        stage.update();
	sf::Vector2u position = getPosition(path, stage);
        if (positionIsOnPath(path, position) && !onPath) {
            onPath = true;
	    offPathSound.stop();
            std::cout << "on path" << std::endl;
        }
        else if (!positionIsOnPath(path, position) && onPath) {
            onPath = false;
	    offPathSound.play();
            std::cout << "off path" << std::endl;
        }
    }
    return 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool findPosition(sf::Image& path, sf::Color color, sf::Vector2u& position)
{
    sf::Vector2u size = path.getSize();

    for (position.x = 0; position.x < size.x; position.x++) {
	for (position.y = 0; position.y < size.y; position.y++) {
	    sf::Color pixel = path.getPixel(position.x, position.y);
	    if ((pixel.r == color.r) &&
		(pixel.g == color.g) &&
		(pixel.b == color.b)) {
		std::cout << position.x << " " << position.y << std::endl;
		return true;
	    }
	}
    }
    return false;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

sf::Vector2u getPosition(sf::Image& path, Stage& stage)
{
    sf::Vector2u size = path.getSize();
    
    std::vector<long> stagePosition = stage.getPosition();
    
    double fractionX = double(stagePosition[0]) / MAX_STEPS_X;
    double fractionY = double(stagePosition[1]) / MAX_STEPS_Y;

    // clamp to [0,1)
    fractionX = fractionX < 0 ? 0 : fractionX;
    fractionX = fractionX > 1 ? float(MAX_STEPS_X-1)/MAX_STEPS_X : fractionX;

    fractionY = fractionY < 0 ? 0 : fractionY;
    fractionY = fractionY > 1 ? float(MAX_STEPS_Y-1)/MAX_STEPS_Y : fractionY;

    sf::Vector2u pathPosition;
    pathPosition.x = fractionX * size.x;
    pathPosition.y = fractionY * size.y;

    return pathPosition;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool positionIsOnPath(sf::Image& path, sf::Vector2u position)
{
    sf::Color pixel = path.getPixel(position.x, position.y);
    if (pixel.r == 255 &&
	pixel.g == 255 &&
	pixel.b == 255)
	return false;
    return true;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

std::vector<long> pathPosToStage(sf::Image& path, sf::Vector2u position)
{
    sf::Vector2u size = path.getSize();
    
    std::vector<long> result;
    
    double fractionX = double(position.x) / size.x;
    double fractionY = double(position.y) / size.y;

    // clamp to [0,1)
    fractionX = fractionX < 0 ? 0 : fractionX;
    fractionX = fractionX > 1 ? float(size.x-1)/size.x : fractionX;

    fractionY = fractionY < 0 ? 0 : fractionY;
    fractionY = fractionY > 1 ? float(size.y-1)/size.y : fractionY;

    
    result.push_back(fractionX * MAX_STEPS_X);
    result.push_back(fractionY * MAX_STEPS_Y);

    return result;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        
