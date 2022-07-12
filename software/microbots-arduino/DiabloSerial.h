#pragma once

#include <Arduino.h>

#ifndef DIABLO_SERIAL_TIMEOUT
#define DIABLO_SERIAL_TIMEOUT 2000
#endif

/* helper macro for timeout loops */
#define WHILE_NOT_TIMEOUT() \
	long start_time = millis(); \
	while (millis() - start_time < DIABLO_SERIAL_TIMEOUT)

class DiabloSerial {
	public:
	enum serial_result_t {
		SR_OK,
		SR_NOACK,
		SR_TIMEOUT,
	};
	DiabloSerial(Stream& serial, int reset_pin) : serial(serial), reset_pin(reset_pin) {
		pinMode(reset_pin, OUTPUT);
	}

	void reset() {
		digitalWrite(reset_pin, 0);
		delay(100);
		digitalWrite(reset_pin, 1);
		delay(3000);
	}

	/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 *
	 * TEXT COMMANDS
	 *
	 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	enum serial_result_t putstr(char *str) {
		int len = strlen(str) + 1;
		return send_command(0x0018, (byte*) str, len);
	}

	enum serial_result_t printf(const char *format, ...) {
		va_list args;
		va_start(args, format);
		char str[256];
		vsprintf(str, format, args);
		va_end(args);

		return putstr(str);
	}

	/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
	 *
	 * GRAPHICS COMMANDS
	 *
	 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	enum serial_result_t gfx_clear_screen() {
		return send_command(0xff82, NULL, 0);
	}

	enum serial_result_t gfx_set_landscape() {
		byte landscape[] = { 0, 0 };
		return send_command(0xff42, landscape, 2);
	}


	/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
	 *
	 * FAT16 COMMANDS
	 *
	 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */
	
	enum serial_result_t fs_get_error(word *error) {
		enum serial_result_t r = send_command(0xfe58, NULL, 0);
		if (r != SR_OK) return r;
		return getword(error);
	}

	enum serial_result_t fs_mount(word *success) {
		enum serial_result_t r = send_command(0xfe3c, NULL, 0);
		if (r != SR_OK) return r;
		return getword(success);
	}

	enum serial_result_t fs_open_file(word *handle, const char *filename, char mode) {
		size_t length = strlen(filename);
		byte data[length + 2];
		memcpy(data, filename, length+1);
		data[length+1] = mode;
		
		enum serial_result_t r = send_command(0x000a, data, length+2);
		if (r != SR_OK) return r;
		return getword(handle);
	}

	enum serial_result_t fs_close_file(word *handle, word *status) {
		byte data[2];
		split(*handle, data+0, data+1);

		enum serial_result_t r = send_command(0xfe51, data, 2);
		if (r != SR_OK) return r;
		return getword(status);
	}

	enum serial_result_t fs_seek_file(word handle, word hi, word lo, word *status) {
		byte data[6];
		split(handle, data+0, data+1);
		split(hi, data+2, data+3);
		split(lo, data+4, data+5);

		enum serial_result_t r = send_command(0xfe4f, data, 6);
		if (r != SR_OK) return r;
		return getword(status);
	}
	
	enum serial_result_t fs_display_image(word handle, word x, word y, word *status) {
		byte data[6];
		split(x, data+0, data+1);
		split(y, data+2, data+3);
		split(handle, data+4, data+5);

		enum serial_result_t r = send_command(0xfe4a, data, 6);
		if (r != SR_OK) return r;
		return getword(status);
	}

	enum serial_result_t fs_seek_display_image(word handle, unsigned long offset, word x, word y, 
	                                           word *status_seek, word *status_display) {
		enum serial_result_t r;
		r = fs_seek_file(handle, offset >> 16, offset, status_seek);
		if (r != SR_OK) return r;
		return fs_display_image(handle, x, y, status_display);
	}

	enum serial_result_t fs_read_file(word handle, byte *dest, size_t n_bytes, word *count) {
		byte data[4];
		split(n_bytes, data+0, data+1);
		split(handle, data+2, data+3);
		enum serial_result_t r = send_command(0x000c, data, 4);
		if (r != SR_OK) return r;

		r = getword(count);
		if (r != SR_OK) return r;

		for (size_t i=0; i<*count; i++) {
			r = getbyte(dest+i);
			if (r != SR_OK) return r;
		}
		return SR_OK;
	}


	/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 *
	 * TOUCH COMMANDS
	 *
	 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	enum serial_result_t touch_enable() {
		byte data[2] = { 0, 0 };
		return send_command(0xfe69, data, 2);
	}
	
	enum touch_status_t {
		NO_TOUCH, PRESS, RELEASE, MOVING, UNKNOWN
	};

	enum serial_result_t touch_get_status(enum touch_status_t *status) {
		byte mode[2] = { 0, 0 }; // get status
		enum serial_result_t r = send_command(0xfe68, mode, 2);
		if (r != SR_OK) return r;
		word s;
		r = getword(&s);
		if (r != SR_OK) return r;
		switch (s) {
			case 0:
				*status = NO_TOUCH;
				break;
			case 1:
				*status = PRESS;
				break;
			case 2:
				*status = RELEASE;
				break;
			case 3:
				*status = MOVING;
				break;
			default:
				*status = UNKNOWN;
				break;
		}
		return SR_OK;
	}

	enum serial_result_t touch_get_position(word *x, word *y) {
		byte data[2] = { 0, 1 }; // get X pos
		enum serial_result_t r = send_command(0xfe68, data, 2);
		if (r != SR_OK) return r;
		r = getword(x);
		if (r != SR_OK) return r;

		data[1] = 2; // get Y pos
		r = send_command(0xfe68, data, 2);
		if (r != SR_OK) return r;
		return getword(y);
	}

	protected:
	Stream& serial;
	int reset_pin;

	/* empty any existing values in serial buffer */
	void empty_serial_buffer() {
		while(serial.available()) serial.read();
	}

	/* read a byte from the screen */
	enum serial_result_t getbyte(byte *b) {
		WHILE_NOT_TIMEOUT() {
			if (serial.available()) {
				if (b != NULL) *b = serial.read();
				return SR_OK;
			}
		}
		return SR_TIMEOUT;
	}

	/* check for ACK byte */
	enum serial_result_t getack() {
		byte b;
		enum serial_result_t r = getbyte(&b);
		if (r != SR_OK) return r;
		if (b != 0x06) return SR_NOACK;
		return SR_OK;
	}

	/* read a word from screen */
	enum serial_result_t getword(word *w) {
		WHILE_NOT_TIMEOUT() {
			if (serial.available() == 2) {
				byte hi = serial.read();
				byte lo = serial.read();
				if (w != NULL)
					*w = (hi << 8) | lo;
				return SR_OK;
			}
		}
		return SR_TIMEOUT;
	}

	/* send word to screen */
	void send_word(word w) {
		serial.write(w >> 8);
		serial.write(w);
	}

	/* split word into two bytes */
	void split(word w, byte *hi, byte *lo) {
		*hi = w >> 8;
		*lo = w;
	}

	/* send command to screen */
	enum serial_result_t send_command(word command, byte *data, size_t n_bytes) {
		empty_serial_buffer();
		send_word(command);
		for (unsigned int i=0; i<n_bytes; i++)
			serial.write(data[i]);
		serial.flush();
		return getack();
	}
};
