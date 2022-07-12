#pragma once
#include "DiabloSerial.h"

#define LOG(prefix, msg) Serial.println(prefix msg)

#define CHECK_ERROR(prefix, r) \
if (r != DiabloSerial::serial_result_t::SR_OK) { \
	Serial.print(prefix "command failed: line "); \
	Serial.println(__LINE__); \
	Serial.print("serial error: "); \
	Serial.println(r); \
	return false; \
}
