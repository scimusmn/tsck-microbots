#include "SerialController.hpp"
#include "Stepper.h"
#include "Button.h"

#define BAUD_RATE 115200

#define PUL_X 2
#define DIR_X 3

#define PUL_Y 4
#define DIR_Y 5

#define BUT_X 6
#define BUT_Y 7

#define JS_UP 10
#define JS_RIGHT 11
#define JS_DOWN 12
#define JS_LEFT 13

SerialController serial;
Button limitX, limitY;
Button jsUp, jsRight, jsDown, jsLeft;
Stepper xAxis, yAxis;
void handleMessage(char* key, char* value);

void setup()
{
    serial.setup(BAUD_RATE, handleMessage);

    // limit switches
    limitX.setup(BUT_X, [](int state) { serial.sendMessage("limit-x", state); });
    limitY.setup(BUT_Y, [](int state) { serial.sendMessage("limit-y", state); });

    // joystick
    jsUp.setup   (JS_UP,    [](int state) { serial.sendMessage("js-up",    1-state); });
    jsRight.setup(JS_RIGHT, [](int state) { serial.sendMessage("js-right", 1-state); });
    jsDown.setup (JS_DOWN,  [](int state) { serial.sendMessage("js-down",  1-state); });
    jsLeft.setup (JS_LEFT,  [](int state) { serial.sendMessage("js-left",  1-state); });

    // stepper motors
    xAxis.setup(PUL_X, DIR_X, 400);
    yAxis.setup(PUL_Y, DIR_Y, 400);
}

void loop()
{
    serial.update();

    limitX.update();
    limitY.update();

    jsUp.update();
    jsRight.update();
    jsDown.update();
    jsLeft.update();

    xAxis.update();
    yAxis.update();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void handleMessage(char* key, char* value)
{
    if (strcmp(key, "wake-arduino") == 0)
        serial.sendMessage("arduino-ready", "1");
    else if (strcmp(key, "x-vel") == 0)
        xAxis.setVelocity(atof(value));
    else if (strcmp(key, "y-vel") == 0)
        yAxis.setVelocity(atof(value));
    else
        serial.sendMessage("unknown-command", "1");
}
