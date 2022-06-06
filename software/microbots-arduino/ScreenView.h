#pragma once

#include <Arduino.h>
#include "DiabloSerial.h"


#define SV_CHECK_ERROR(r) CHECK_ERROR("ScreenView: ", r)
#define SV_LOG(msg) LOG("[ScreenView] ", msg)


class ScreenView {
	public:
	ScreenView(DiabloSerial& screen) : screen(screen) {}
	void init() {
		word error;	
		enum DiabloSerial::serial_result_t r;

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

	void displayHeart() {
		screen.fs_seek_display_image(imgFile, heart, 340, 61, NULL, NULL);
	}

	void displayHealth(int health) {
		/* step: 53 */
		for (int i=0; i<8; i++) {
			int x = 585 - (53*i);
			long image = (health > i) ? health_good : health_bad;
			screen.fs_seek_display_image(imgFile, image, x, 196, NULL, NULL);
		}
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
};
