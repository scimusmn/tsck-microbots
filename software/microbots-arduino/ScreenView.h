#pragma once

#include <Arduino.h>
#include "DiabloSerial.h"
#include "offsets.h"
#include "CheckError.h"


#define SV_CHECK_ERROR(r) CHECK_ERROR("ScreenView: ", r)
#define SV_LOG(msg) LOG("[ScreenView] ", msg)


class ScreenView {
	public:
	ScreenView(DiabloSerial& screen) : screen(screen) {}
	bool init() {
		word error;	
		enum DiabloSerial::serial_result_t r;

		SV_LOG("Reset screen");
		screen.reset();
		SV_LOG("Clear screen");
		r = screen.gfx_clear_screen();
		SV_CHECK_ERROR(r);
		SV_LOG("Set landscape");
		r = screen.gfx_set_landscape();
		SV_CHECK_ERROR(r);
		delay(10); // give the screen time to adjust

		SV_LOG("Mount filesystem");
		r = screen.fs_mount(&error);
		SV_CHECK_ERROR(r);
		if (error == 0) {
			Serial.println("failed to mount filesystem!");
			return false;
		}


		SV_LOG("Enable touch");
		r = screen.touch_enable();
		SV_CHECK_ERROR(r);
		
		SV_LOG("Open image file");
		r = screen.fs_open_file(&imgFile, "images.gci", 'r');
		SV_CHECK_ERROR(r);

		displayAttractScreen();
	}

	bool touched() {
		enum DiabloSerial::serial_result_t r;
		enum DiabloSerial::touch_status_t status;
		r = screen.touch_get_status(&status);
		SV_CHECK_ERROR(r);
		if (status == DiabloSerial::touch_status_t::PRESS) return true;
		return false;
	}

	void displayAttractScreen() {
		screen.fs_seek_display_image(imgFile, attract_screen, 0, 0, NULL, NULL);
	}

	void displayGameOverScreen() {
		screen.fs_seek_display_image(imgFile, game_over_screen, 0, 0, NULL, NULL);
	}

	void displaySuccessScreen() {
		screen.fs_seek_display_image(imgFile, success_screen, 0, 0, NULL, NULL);
	}

	void displayResetScreen() {
		screen.fs_seek_display_image(imgFile, resetting_screen, 0, 0, NULL, NULL);
	}

	void displayBackground() {
		screen.fs_seek_display_image(imgFile, background, 0, 0, NULL, NULL);
	}

	void displayHealth(int health) {
		int index = health;
		if (index < 0) index = 0;
		if (index > 9) index = 9;
		screen.fs_seek_display_image(imgFile, heart[index], 232, 97, NULL, NULL);
	}

	void displayDigits(int d0, int d1, int d2, int d3) {
	    screen.fs_seek_display_image(imgFile, digit0[d0],  22, 415, NULL, NULL);
	    screen.fs_seek_display_image(imgFile, digit1[d1],  59, 415, NULL, NULL);
	    screen.fs_seek_display_image(imgFile, colon,       95, 415, NULL, NULL);
	    screen.fs_seek_display_image(imgFile, digit2[d2], 116, 415, NULL, NULL);
	    screen.fs_seek_display_image(imgFile, digit3[d3], 153, 415, NULL, NULL);
	}

	protected:
	DiabloSerial& screen;
	word imgFile;
	enum ViewState {
		ATTRACT,
		RESET,
		PLAYING,
		GAME_OVER,
		GAME_SUCCESS,
	} viewState;

	unsigned long digit0[10] = { digit0_0x0, digit0_1x0, digit0_2x0, digit0_3x0, digit0_4x0,
	                             digit0_5x0, digit0_6x0, digit0_7x0, digit0_8x0, digit0_9x0 };
	unsigned long digit1[10] = { digit1_0x0, digit1_1x0, digit1_2x0, digit1_3x0, digit1_4x0,
	                             digit1_5x0, digit1_6x0, digit1_7x0, digit1_8x0, digit1_9x0 };
	unsigned long digit2[10] = { digit2_0x0, digit2_1x0, digit2_2x0, digit2_3x0, digit2_4x0,
	                             digit2_5x0, digit2_6x0, digit2_7x0, digit2_8x0, digit2_9x0 };
	unsigned long digit3[10] = { digit3_0x0, digit3_1x0, digit3_2x0, digit3_3x0, digit3_4x0,
	                             digit3_5x0, digit3_6x0, digit3_7x0, digit3_8x0, digit3_9x0 };
	unsigned long heart[10] = { health0, health1, health2, health3, health4,
	                            health5, health6, health7, health8, health9 };
};
