#pragma once

#include <Arduino.h>
#include "offsets.h"
#include "DiabloSerial.h"
#include "CheckError.h"
#include "ScreenView.h"
#include "Path.h"
#include "Motor.h"
#include "XYTable.h"
#include "Joystick.h"
#include "IntervalTimer.h"

/* screen constants */
#define RESET_PIN 13
#define SERIAL Serial1

/* space constants */
#define STEPS_X 17500
#define STEPS_Y 14000

/* motor constants */
#define MAGNET_A 5
#define MAGNET_B 4

/* joystick constants */
#define JOY_UP 9
#define JOY_RIGHT 10
#define JOY_DOWN 11
#define JOY_LEFT 12

/* logging macros */
#define GM_LOG(msg) LOG("[GameModel] ", msg)

#define GC_CHECK_ERROR(r) CHECK_ERROR("GameController: ", r)
#define GC_LOG(msg) LOG("[GameController] ", msg)

static int distance(PathPoint a, PathPoint b) {
	const int dx = ((int)a.x) - ((int)b.x);
	const int dy = ((int)a.y) - ((int)b.y);
	return (dx*dx) + (dy*dy);
}


class GameModel {
	public:
	enum GameState {
		ATTRACT_SCREEN,
		RESETTING,
		PLAYING,
	} gameState;

	GameModel(DiabloSerial& screen) : screen(screen), health(16) {}
	void init() {
		gameState = ATTRACT_SCREEN;
		GM_LOG("Load path");
		path.load(screen, "path.bin");
		endPoint = path.getPathPoint(path.getNumPoints()-1);
	}

	void startGame() {
		gameState = PLAYING;
		health = 8;
		startTime = millis();
	}

	PathPoint getPathPoint(int index) {
		return path.getPathPoint(index);
	}

	int getNumPoints() {
		return path.getNumPoints();
	}

	int isObstructed(int x, int y) {
		return path.isObstructed(x, y);
	}

	void dump() {
		GM_LOG("Path:");
		path.print_path();
		GM_LOG("Obstructions:");
		path.print_obstruction();
	}

	void reduceHealth() {
		health -= 1;
		GM_LOG("Health reduced!");
	}

	int getHealth() {
		return health;
	}

	void getTime(int *minutes, int *seconds) {
		unsigned long s = (millis() - startTime) / 1000;
		*seconds = s % 60;
		*minutes = s / 60;
	}

	protected:
	DiabloSerial& screen;
	Path path;
	PathPoint endPoint;
	int health;
	unsigned long startTime;
};



class GameController {
	public:
	GameController() 
		: screen(SERIAL, RESET_PIN), model(screen), 
		  screenView(screen), 
		  magnets(MAGNET_A, MAGNET_B),
		  joy(JOY_UP, JOY_RIGHT, JOY_DOWN, JOY_LEFT),
		  updateTimeView(1000), updateHealth(1000), updateTouch(200)
	{}

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

		model.dump();

		GC_LOG("Home axes");
		magnets.runRight();
		table.init();

		GC_LOG("Go to start");
		PathPoint start = model.getPathPoint(0);
		table.moveTo(pixelsToSteps(start.x, STEPS_X), pixelsToSteps(start.y, STEPS_Y), 400);
		magnets.stop();

		GC_LOG("Init complete");
	}

	void updatePlaying() {
		joy.update();
		long x, y;
		table.getPosition(&x, &y);
		x = stepsToPixels(x, STEPS_X);
		y = stepsToPixels(y, STEPS_Y);

		if (updateTouch.triggered()) {
			// check for touch
			if (screenView.touched()) {
				screenView.displayResetScreen();
				model.gameState = GameModel::GameState::RESETTING;
				return;
			}
		}

		if (updateTimeView.triggered()) {
			// update time display
			int min, sec;
			model.getTime(&min, &sec);
			screenView.displayDigits(min/10, min%10, sec/10, sec%10);
		}

		if (updateHealth.triggered() && model.isObstructed(x, y)) {
			model.reduceHealth();
			screenView.displayHealth(model.getHealth());
		}

		if (model.getHealth() <= 0) {
			magnets.stop();
			screenView.displayGameOverScreen();
			delay(5000);
			screenView.displayResetScreen();
			model.gameState = GameModel::GameState::RESETTING;
			return;
		}

		if (joy.hasChanged()) {
			int dir = joy.getDirection();
			Serial.println(dir, BIN);

			if (dir & Joystick::LEFT) magnets.runLeft();
			else if (dir & Joystick::RIGHT) magnets.runRight();
			else if (dir & Joystick::UP) magnets.runLeft();
			else if (dir & Joystick::DOWN) magnets.runRight();
			else magnets.stop();
			
			int vx = getMotorSpeed(dir, Joystick::LEFT, Joystick::RIGHT);
			int vy = getMotorSpeed(dir, Joystick::UP, Joystick::DOWN);
			table.setSpeed(vx, vy);
		}
		/* update motors */
		table.update();
	}

	void startGame() {
		model.startGame();
		screenView.displayBackground();
		screenView.displayHeart();
		screenView.displayHealth(8);
		screenView.displayDigits(0, 0, 0, 0);
		updateTimeView.start();
		updateHealth.start();
		updateTouch.start();
	}

	void updateAttract() {
		if (screenView.touched())
			startGame();
	}

	void updateReset() {
		long x, y;
		table.getPosition(&x, &y);
		x = stepsToPixels(x, STEPS_X);
		y = stepsToPixels(y, STEPS_Y);

		GC_LOG("Find closest path point");
		PathPoint current = { x, y };
		int closest = 0;
		int minDist = 10000;
		for (int i=0; i<model.getNumPoints(); i++) {
			PathPoint pt = model.getPathPoint(i);
			int dist = distance(current, pt);
			Serial.print(i); Serial.print(" ");
			Serial.print("("); Serial.print(pt.x); Serial.print(", "); Serial.print(pt.y); Serial.print("): ");
			Serial.println(dist);
			if (dist < minDist) {
				Serial.println("closest?");
				minDist = dist;
				closest = i;
			}
		}

		GC_LOG("Move to path");
		Serial.println(closest);
		magnets.runRight();
		for (int i=closest; i>=0; i--) {
			PathPoint pt = model.getPathPoint(i);
			Serial.print("[GameController] Next path point: (");
			Serial.print(pt.x); Serial.print(", "); Serial.print(pt.y); Serial.println(")");
			table.moveTo(pixelsToSteps(pt.x, STEPS_X), pixelsToSteps(pt.y, STEPS_Y), 400);
		}
		magnets.stop();

		GC_LOG("Return to attract screen");

		model.gameState = GameModel::GameState::ATTRACT_SCREEN;
		screenView.displayAttractScreen();
	}

	void update() {
		switch(model.gameState) {
			case GameModel::GameState::ATTRACT_SCREEN:
				updateAttract();
				break;
			case GameModel::GameState::RESETTING:
				updateReset();
				break;
			case GameModel::GameState::PLAYING:
				updatePlaying();
				break;
			default:
				GC_LOG("Bad game state, resetting to attract");
				screenView.displayAttractScreen();
				model.gameState = GameModel::GameState::ATTRACT_SCREEN;
				break;
		}
	}

	protected:
	GameModel model;
	ScreenView screenView;
	XYTable table;
	MagnetMotor magnets;
	DiabloSerial screen;
	Joystick joy;

	IntervalTimer updateTimeView, updateHealth, updateTouch;

	int stepsToPixels(long x, long maxSteps) {
		return 127 - ((128*x)/maxSteps);	
	}

	int pixelsToSteps(long x, long maxSteps) {
		return maxSteps - ((maxSteps*x)/128);
	}

	int getMotorSpeed(int dir, int pos, int neg) {
		if ((!(dir & pos)) && (!(dir & neg))) {
			// neither direction is pressed
			return 0;
		}
		else if (dir & pos) {
			return 400;
		}
		else {
			return -400;
		}
	}
};
