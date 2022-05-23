#include <SoftwareSerial.h>

#include "Screen.h"

#define TX_PIN 3
#define RX_PIN 2
#define RST_PIN 4

smm::Screen screen(RX_PIN, TX_PIN, RST_PIN);

void setup() {
	Serial.begin(115200);
	Serial.println("=== ARDUINO BOOT ===");
	screen.setup();
}

void loop() {
    // pass
}
