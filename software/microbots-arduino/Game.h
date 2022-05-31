#pragma once

#include <Arduino.h>
#include "offsets.h"
#include "DiabloSerial.h"
#include "CheckError.h"
#include "Path.h"

#define RESET_PIN 4
#define SERIAL Serial1

#define GM_LOG(msg) LOG("[GameModel] ", msg)

#define GC_CHECK_ERROR(r) CHECK_ERROR("GameController: ", r)
#define GC_LOG(msg) LOG("[GameController] ", msg)

#define SV_CHECK_ERROR(r) CHECK_ERROR("ScreenView: ", r)
#define SV_LOG(msg) LOG("[ScreenView] ", msg)


class GameModel {
	public:
	GameModel(DiabloSerial& screen) : screen(screen), health(16) {}
	void init() {
		GM_LOG("Load path");
		path.load(screen, "path.bin");
	}

	PathPoint getPathPoint(int index) {
		return path.getPathPoint(index);
	}

	int isObstructed(int x, int y) {
		path.isObstructed(x, y);
	}

	void dump() {
		GM_LOG("Path:");
		path.print_path();
		GM_LOG("Obstructions:");
		path.print_obstruction();
	}

	protected:
	DiabloSerial& screen;
	Path path;
	int health;
};


class ScreenView {
	public:
	ScreenView(DiabloSerial& screen) : screen(screen) {}
	void init() {
		word error;	
		enum DiabloSerial::serial_result_t r;
		
		SV_LOG("Open image file");
		r = screen.fs_open_file(&imgFile, "images.gci", 'r');
		SV_CHECK_ERROR(r);
	}

	void displayBackground() {
		screen.fs_seek_display_image(imgFile, background, 0, 0, NULL, NULL);
	}

	void displayDigits(int d0, int d1, int d2, int d3) {
	    screen.fs_seek_display_image(imgFile, digit0[d0], 192, 297, NULL, NULL);
	    screen.fs_seek_display_image(imgFile, digit1[d1], 273, 297, NULL, NULL);
	    screen.fs_seek_display_image(imgFile, colon,      354, 297, NULL, NULL);
	    screen.fs_seek_display_image(imgFile, digit2[d2], 435, 297, NULL, NULL);
	    screen.fs_seek_display_image(imgFile, digit3[d3], 516, 297, NULL, NULL);
	}

	protected:
	DiabloSerial& screen;
	word imgFile;

	unsigned long digit0[10] = { digit0_0x0, digit0_1x0, digit0_2x0, digit0_3x0, digit0_4x0,
	                             digit0_5x0, digit0_6x0, digit0_7x0, digit0_8x0, digit0_9x0 };
	unsigned long digit1[10] = { digit1_0x0, digit1_1x0, digit1_2x0, digit1_3x0, digit1_4x0,
	                             digit1_5x0, digit1_6x0, digit1_7x0, digit1_8x0, digit1_9x0 };
	unsigned long digit2[10] = { digit2_0x0, digit2_1x0, digit2_2x0, digit2_3x0, digit2_4x0,
	                             digit2_5x0, digit2_6x0, digit2_7x0, digit2_8x0, digit2_9x0 };
	unsigned long digit3[10] = { digit3_0x0, digit3_1x0, digit3_2x0, digit3_3x0, digit3_4x0,
	                             digit3_5x0, digit3_6x0, digit3_7x0, digit3_8x0, digit3_9x0 };
};


class GameController {
	public:
	GameController() : screen(SERIAL, RESET_PIN), model(screen), screenView(screen) {}

	init() {
		SERIAL.begin(9600);
		enum DiabloSerial::serial_result_t r;

		GC_LOG("Reset screen");
		r = screen.reset();
		GC_CHECK_ERROR(r);
		GC_LOG("Clear screen");
		r = screen.gfx_clear_screen();
		GC_CHECK_ERROR(r);
		GC_LOG("Set landscape");
		r = screen.gfx_set_landscape();
		GC_CHECK_ERROR(r);
		delay(10); // give the screen time to adjust

		word error;
		GC_LOG("Mount filesystem");
		r = screen.fs_mount(&error);
		GC_CHECK_ERROR(r);
		if (error == 0) {
			Serial.println("failed to mount filesystem!");
			return;
		}

		screenView.init();
		model.init();

		screenView.displayBackground();
		model.dump();
		GC_LOG("Init complete");
	}

	protected:
	GameModel model;
	ScreenView screenView;
	DiabloSerial screen;
};
