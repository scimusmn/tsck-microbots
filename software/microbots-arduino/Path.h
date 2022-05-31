#include <Arduino.h>
#include "DiabloSerial.h"

#define CHECK_ERR(r) if (r != DiabloSerial::serial_result_t::SR_OK) { \
	Serial.print("command failed!"); \
	Serial.println(r); \
	Serial.println(__LINE__); \
	return false; \
}

class Path {
	public:
	Path() {}

	bool load(DiabloSerial& serial, const char * filename) {
		word handle = 0;
		enum DiabloSerial::serial_result_t r;
		r = serial.fs_open_file(&handle, filename, 'r');
		Serial.println(handle);
		CHECK_ERR(r);

		/* read path info */
		n_points = 0;
		word status;
		r = serial.fs_read_file(handle, &n_points, 1, &status);
		CHECK_ERR(r);
		r = serial.fs_seek_file(handle, 0, 1, &status);
		CHECK_ERR(r);
		r = serial.fs_read_file(handle, (byte*) path, 2*n_points, &status);
		CHECK_ERR(r);

		/* read obstruction info */
		int offset = 2*n_points + 1;
		r = serial.fs_seek_file(handle, 0, offset, &status);
		CHECK_ERR(r);
		r = serial.fs_read_file(handle, obstruction, 512, &status);
		CHECK_ERR(r);
		offset += 512;
		r = serial.fs_seek_file(handle, 0, offset, &status);
		CHECK_ERR(r);
		r = serial.fs_read_file(handle, obstruction+512, 512, &status);
		CHECK_ERR(r);
		offset += 512;
		r = serial.fs_seek_file(handle, 0, offset, &status);
		CHECK_ERR(r);
		r = serial.fs_read_file(handle, obstruction+1024, 512, &status);
		CHECK_ERR(r);
		offset += 512;
		r = serial.fs_seek_file(handle, 0, offset, &status);
		CHECK_ERR(r);
		r = serial.fs_read_file(handle, obstruction+1536, 512, &status);
		CHECK_ERR(r);

		/* close file */
		r = serial.fs_close_file(handle, &status);
		CHECK_ERR(r);
		return true;
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

	void print_obstruction() {
		for (int y=0; y<128; y++) {
			for (int i=0; i<16; i++) {
				Serial.print(obstruction[16*y + i], BIN);
				Serial.print(" ");
			}
			Serial.println();
		}
	}

	private:
	byte n_points;
	struct path_point {
		byte x, y;
	} path[128];

	byte obstruction[2048];
};
