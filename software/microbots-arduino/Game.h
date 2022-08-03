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
#include "Buzzer.h"

/* screen constants */
#define RESET_PIN 21
#define SERIAL Serial1

/* space constants */
#define STEPS_X 17500
#define STEPS_Y 14700

/* motor constants */
#define MAGNET_A 7
#define MAGNET_B 6

/* joystick constants */
#define JOY_UP 20
#define JOY_RIGHT 13
#define JOY_DOWN 12
#define JOY_LEFT 11

/* buzzer constants */
#define BUZZER_PIN 8
const SongNote winningSong[] = {
    { Note::C, 100 },
    { Note::E, 100 },
    { Note::G, 100 },
    { Note::E, 100 },
    { Note::C5, 500 },	 
};
const SongNote losingSong[] = {
    { Note::G, 200 },
    { Note::Eb, 250 },
    { Note::D, 300 },
    { Note::C, 500 },
};

/* logging macros */
#define GM_LOG(msg) LOG("[GameModel] ", msg)

#define GC_CHECK_ERROR(r) CHECK_ERROR("GameController: ", r)
#define GC_LOG(msg) LOG("[GameController] ", msg)

static long distance(Position a, Position b) {
	const long dx = (a.x) - (b.x);
	const long dy = (a.y) - (b.y);
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
		endPoint = path.getPoint(path.getNumPoints()-1);
	}

	void startGame() {
		gameState = PLAYING;
		health = 9;
		startTime = millis();
		alertNote = Note::C;
	}

	Position getPoint(int index) {
		return path.getPoint(index);
	}

	int getNumPoints() {
		return path.getNumPoints();
	}

	int isObstructed(Position pixelPos) {
		if (pixelPos.x < 0 || pixelPos.x > 127 ||
		    pixelPos.y < 0 || pixelPos.y > 127) {
			// position out of range
			return 1;
		}
		return path.isObstructed(pixelPos.x, pixelPos.y);
	}

	Position getEndPoint() {
		return endPoint;
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

	Note getAlertNote() {
		alertNote = (Note) ( ((int) alertNote) + 1 );
		return alertNote;
	}

	protected:
	DiabloSerial& screen;
	Path path;
	Position endPoint;
	int health;
	unsigned long startTime;
	Note alertNote;
};



class GameController {
	public:
	GameController() 
		: screen(SERIAL, RESET_PIN), model(screen), 
		  screenView(screen), 
		  magnets(MAGNET_A, MAGNET_B),
		  joy(JOY_UP, JOY_RIGHT, JOY_DOWN, JOY_LEFT),
		  updateTimeView(1000), updateHealth(1000), updateTouch(200),
		  buzzer(BUZZER_PIN),
		  moving(false),
		  table(STEPS_X, STEPS_Y)
	{}

	init() {
		SERIAL.begin(9600);
		enum DiabloSerial::serial_result_t r;

		screenView.init();
		model.init();

		model.dump();

		GC_LOG("Home axes");
		magnets.runRight();
		table.init();

		GC_LOG("Go to start");
		Position start = model.getPoint(0);
		start = table.toSteps(start);
		table.moveTo(start.x, start.y, 400);
		magnets.stop();

		GC_LOG("Init complete");
	}

	void updatePlaying() {
		joy.update();
		Position steps = table.getPosition();
		Position pixels = table.toPixels(steps);

		Position end = model.getEndPoint();
		if (distance(pixels, end) < 4) {
			magnets.stop();
			screenView.displaySuccessScreen();
			buzzer.playSong(winningSong, N_NOTES(winningSong));
			delay(5000);
			screenView.displayResetScreen();
			model.gameState = GameModel::GameState::RESETTING;
			return;
		}

		if ((!moving) && updateTouch.triggered()) {
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

		static bool beeping = false;
		if (model.isObstructed(pixels)) {
			if (!beeping) {
				buzzer.play(Note::E);
				beeping = true;
			}
		}
		else {
			if (beeping) {
				buzzer.quiet();
				beeping = false;
			}
		}

		if (updateHealth.triggered() && model.isObstructed(pixels)) {
			model.reduceHealth();
			screenView.displayHealth(model.getHealth());
		}

		if (model.getHealth() <= 0) {
			magnets.stop();
			screenView.displayGameOverScreen();
			buzzer.playSong(losingSong, N_NOTES(losingSong));
			delay(5000);
			screenView.displayResetScreen();
			model.gameState = GameModel::GameState::RESETTING;
			return;
		}



		if (joy.hasChanged()) {
			int dir = joy.getDirection();
			if (dir != 0) moving = true;
			else moving = false;
			Serial.println(dir, BIN);

			if (dir & Joystick::LEFT) magnets.runLeft();
			else if (dir & Joystick::RIGHT) magnets.runRight();
			else if (dir & Joystick::UP) magnets.runLeft();
			else if (dir & Joystick::DOWN) magnets.runRight();
			else magnets.stop();
			
			int vx = getMotorSpeed(dir, Joystick::LEFT, Joystick::RIGHT);
			#ifdef MIRROR_TABLE
			vx = -vx;
			#endif
			int vy = getMotorSpeed(dir, Joystick::UP, Joystick::DOWN);
			Serial.print(vx); Serial.print(", "); Serial.println(vy);
			table.setSpeed(vx, vy);
		}
		/* update motors */
		table.update();
	}

	void startGame() {
		model.startGame();
		screenView.displayBackground();
		screenView.displayHealth(9);
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
		Position steps = table.getPosition();
		Position current = table.toPixels(steps);

		GC_LOG("Find closest path point");
		int closest = 0;
		unsigned int minDist = 100000;
		for (int i=0; i<model.getNumPoints(); i++) {
			Position pt = model.getPoint(i);
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
			Position pt = model.getPoint(i);
			Serial.print("[GameController] Next path point: (");
			Serial.print(pt.x); Serial.print(", "); Serial.print(pt.y); Serial.println(")");
			pt = table.toSteps(pt);
			table.moveTo(pt.x, pt.y, 800);
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
	Buzzer buzzer;
	IntervalTimer updateTimeView, updateHealth, updateTouch;
	bool moving;

	int getMotorSpeed(int dir, int pos, int neg) {
		if ((!(dir & pos)) && (!(dir & neg))) {
			// neither direction is pressed
			return 0;
		}
		else if (dir & pos) {
			return 800;
		}
		else {
			return -800;
		}
	}
};
