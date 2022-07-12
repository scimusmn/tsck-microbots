#pragma once

#include <Arduino.h>
#include "DiabloSerial.h"
#include "CheckError.h"
#include "Position.h"

#define PATH_CHECK_ERR(r) CHECK_ERROR("Path: ", r)
#define PATH_LOG(msg) LOG("[Path] ", msg)

struct PathPoint {
	byte x, y;
};


class Path {
	public:
	Path() {}

	bool load(DiabloSerial& serial, const char * filename) {
		word handle = 0;
		enum DiabloSerial::serial_result_t r;
		PATH_LOG("Open path file");
		r = serial.fs_open_file(&handle, filename, 'r');
		PATH_CHECK_ERR(r);

		/* read path info */
		PATH_LOG("Read path data");
		n_points = 0;
		word status;
		r = serial.fs_read_file(handle, &n_points, 1, &status);
		PATH_CHECK_ERR(r);
		r = serial.fs_seek_file(handle, 0, 1, &status);
		PATH_CHECK_ERR(r);
		r = serial.fs_read_file(handle, (byte*) path, 2*n_points, &status);
		PATH_CHECK_ERR(r);

		/* read obstruction info */
		PATH_LOG("Read obstruction data");
		int offset = 2*n_points + 1;
		r = serial.fs_seek_file(handle, 0, offset, &status);
		PATH_CHECK_ERR(r);
		r = serial.fs_read_file(handle, obstruction, 512, &status);
		PATH_CHECK_ERR(r);
		PATH_LOG("Done: read block 0");
		
		offset += 512;
		r = serial.fs_seek_file(handle, 0, offset, &status);
		PATH_CHECK_ERR(r);
		r = serial.fs_read_file(handle, obstruction+512, 512, &status);
		PATH_CHECK_ERR(r);
		PATH_LOG("Done: read block 1");

		offset += 512;
		r = serial.fs_seek_file(handle, 0, offset, &status);
		PATH_CHECK_ERR(r);
		r = serial.fs_read_file(handle, obstruction+1024, 512, &status);
		PATH_CHECK_ERR(r);
		PATH_LOG("Done: read block 2");
		
		offset += 512;
		r = serial.fs_seek_file(handle, 0, offset, &status);
		PATH_CHECK_ERR(r);
		r = serial.fs_read_file(handle, obstruction+1536, 512, &status);
		PATH_CHECK_ERR(r);
		PATH_LOG("Done: read block 3");

		/* close file */
		r = serial.fs_close_file(handle, &status);
		PATH_CHECK_ERR(r);
		return true;
	}

	void printByte(byte b) {
		for (int i=7; i>=0; i--) {
			Serial.print((b >> i) & 0x01);
		}
	}

	void print_path() {
		for (int i=0; i<n_points; i++) {
			Serial.print("(");
			Serial.print(path[i].x);
			Serial.print(", ");
			Serial.print(path[i].y);
			Serial.println(")");
		}
	}

	int getNumPoints() {
		return n_points;
	}

	Position getPoint(int index) {
		if (index < 0) {
			index = n_points - index;
		}
		Position pos;
		pos.x = path[index].x;
		pos.y = path[index].y;
		return pos;
	}

	int isObstructed(int x, int y) {
		int rowOffset = 16*y;
		int colOffset = x/8;
		int bitOffset = 7 - (x%8);
		return obstruction[rowOffset + colOffset] & (1 << bitOffset);
	}

	void print_obstruction() {
		for (int y=0; y<128; y++) {
			for (int i=0; i<16; i++) {
				printByte(obstruction[16*y + i]);
				Serial.print(" ");
			}
			Serial.println();
		}
	}

	private:
	byte n_points;
	PathPoint path[128];

	byte obstruction[2048];
};
