#include <SoftwareSerial.h>

#include "Game.h"

GameController game;

void setup() {
	Serial.begin(115200);
	game.init();
}

void loop() {
	game.update();
}
