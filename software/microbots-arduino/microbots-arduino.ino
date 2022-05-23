#include <SoftwareSerial.h>

#include "DiabloSerial.h"
#include "offsets.h"

#define TX_PIN 3
#define RX_PIN 2
#define RST_PIN 4

SoftwareSerial diablo(RX_PIN, TX_PIN);
DiabloSerial screen(diablo, RST_PIN);

#define CHECK_ERROR() \
	if (r != DiabloSerial::serial_result_t::SR_OK) serial_printf("ERROR: %d", r);

void serial_printf(const char *format, ...) {
	va_list args;
	va_start(args, format);
	char buffer[128];
	vsprintf(buffer, format, args);
	va_end(args);
	Serial.print(buffer);
	Serial.flush();
}


word images;
int index = 0;
unsigned long digit0[] = { digit0_0x0, digit0_1x0, digit0_2x0, digit0_3x0, digit0_4x0,
                           digit0_5x0, digit0_6x0, digit0_7x0, digit0_8x0, digit0_9x0 };
unsigned long digit1[] = { digit1_0x0, digit1_1x0, digit1_2x0, digit1_3x0, digit1_4x0,
                           digit1_5x0, digit1_6x0, digit1_7x0, digit1_8x0, digit1_9x0 };
unsigned long digit2[] = { digit2_0x0, digit2_1x0, digit2_2x0, digit2_3x0, digit2_4x0,
                           digit2_5x0, digit2_6x0, digit2_7x0, digit2_8x0, digit2_9x0 };
unsigned long digit3[] = { digit3_0x0, digit3_1x0, digit3_2x0, digit3_3x0, digit3_4x0,
                           digit3_5x0, digit3_6x0, digit3_7x0, digit3_8x0, digit3_9x0 };


void display_digits(int d0, int d1, int d2, int d3) {
	screen.fs_seek_display_image(images, digit0[d0], 192, 297, NULL, NULL);
	screen.fs_seek_display_image(images, digit1[d1], 273, 297, NULL, NULL);
	screen.fs_seek_display_image(images, colon,      354, 297, NULL, NULL);
	screen.fs_seek_display_image(images, digit2[d2], 435, 297, NULL, NULL);
	screen.fs_seek_display_image(images, digit3[d3], 516, 297, NULL, NULL);
}

void display_time(unsigned long start) {
	unsigned long t = (millis() - start) / 1000;
	unsigned long seconds = t % 60;
	unsigned long minutes = t / 60;

	int m0 = minutes / 10;
	int m1 = minutes % 10;
	int s0 = seconds / 10;
	int s1 = seconds % 10;
	display_digits(m0, m1, s0, s1);
}

void setup() {
	enum DiabloSerial::serial_result_t r;

	Serial.begin(115200);
	diablo.begin(9600);
	Serial.println("=== ARDUINO BOOT ===");

	serial_printf("reset screen\n");
	r = screen.reset();
	CHECK_ERROR();

	serial_printf("clear screen\n");
	r = screen.gfx_clear_screen();
	CHECK_ERROR();

	serial_printf("set landscape\n");
	r = screen.gfx_set_landscape();
	CHECK_ERROR();

	word error;
	serial_printf("mount fs\n");
	r = screen.fs_mount(&error);
	CHECK_ERROR();
	serial_printf("\t%d\n", error);

	serial_printf("open image\n");
	r = screen.fs_open_file(&images, "images.gci", 'r');
	CHECK_ERROR();
	serial_printf("\t%d\n", images);

	r = screen.fs_get_error(&error);
	CHECK_ERROR();
	serial_printf("file error: %d\n", error);

	screen.touch_enable();

	screen.fs_seek_display_image(images, background, 0, 0, NULL, NULL);
}

void loop() {
	static unsigned long start = millis();

	display_time(start);
	delay(20);
}
