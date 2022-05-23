#pragma once

#include <SoftwareSerial.h>

#include <Diablo_Const4D.h>
#include <Diablo_Serial_4DLib.h>

#define SERIAL_TIMEOUT 2000

namespace smm {
	class Screen {
		public:
		Screen(int rxPin, int txPin, int resetPin,
		       unsigned int width=800,
		       unsigned int height=480)
			: mSerial(rxPin, txPin),
			  mResetPin(resetPin)
		{}

		void setup()
		{
			/* initialize software serial */
			mSerial.begin(9600);

			/* ensure screen is reset */
			pinMode(mResetPin, OUTPUT);
			digitalWrite(mResetPin, 0);
			delay(100);
			digitalWrite(mResetPin, 1);
			delay(3000);

			enum serial_result_t r;

			/* set screen to landscape mode */
			word landscape = 0;
			r = command_response(0xff42, &landscape, 1, NULL);
			if (r != SR_OK)
				serial_printf("screen mode failed: %d\n", r);
			else
				serial_printf("set screen to landscape\n");

			/* clear screen */
			r = command(0xff82, NULL, 0);
			if (r != SR_OK)
				serial_printf("clear screen failed: %d\n", r);
			else
				serial_printf("cleared screen\n");

			/* mount FAT16 fs */
			word success;
			r = command_response(0xfe3c, NULL, 0, &success);
			if (r != SR_OK)
				serial_printf("mount fs failed: %d\n", r);
			else if (success == 0)
				serial_printf("mount fs failed!\n");
			else
				serial_printf("mounted fs\n");


			/* open image file */
			word handle;
			const char *fname = "images.gci";
			r = open_file(fname, 'r', &handle);
			if (r != SR_OK)
				serial_printf("open '%s' failed: %d\n", fname, r);
			else if (handle == 0)
				serial_printf("could not find file '%s'\n", fname);
			else
				serial_printf("opened '%s'\n", fname);

			/* seek to image */
			word seek_args[] = {
				handle,
				0x0000,
				0x59e2,
			};
			word error;
			r = command_response(0xfe4f, seek_args, 3, &error);
			if (r != SR_OK)
				serial_printf("file seek failed: %d\n", r);
			else if (error != 0)
				serial_printf("file seek failed due to file error: %d\n", error);
			else
				serial_printf("file seek done\n");


			/* display image */
			error;
			word xy_handle[] = {
				0x0100, /* x pos */
				0x0100, /* y pos */
				handle,
			};
			r = command_response(0xfe4a, xy_handle, 3, &error);
			if (r != SR_OK)
				serial_printf("display image failed: %d\n", r);
			else if (error != 0)
				serial_printf("display image failed due to file error: %d\n", error);
			else
				serial_printf("displayed image\n");
		}


		private:
		SoftwareSerial mSerial;
		int mResetPin;

		enum serial_result_t {
			SR_OK,
			SR_NOACK,
			SR_TIMEOUT,
		};

		void send_word(word w) {
			mSerial.write(w >> 8);
			mSerial.write(w);
		}

		/* destroy existing values in the serial buffer */
		void empty_serial_buffer() {
			while(mSerial.available()) Serial.read();
		}

		enum serial_result_t getack() {
			long start_time = millis();
			while (millis() - start_time < SERIAL_TIMEOUT) {
				if (mSerial.available()) {
					byte b = mSerial.read();
					if (b == 0x06) return SR_OK;
					else return SR_NOACK;
				}
			}
			return SR_TIMEOUT;
		}

		enum serial_result_t getword(word *w) {
			long start_time = millis();
			while (millis() - start_time < SERIAL_TIMEOUT) {
				if (mSerial.available() == 2) {
					byte hi = mSerial.read();
					byte lo = mSerial.read();
					if (w != NULL)
						*w = (hi << 8) | lo;
					return SR_OK;
				}
			}
			return SR_TIMEOUT;
		}

		enum serial_result_t command(word c, word *data, size_t num) {
			empty_serial_buffer();
			send_word(c);
			for (int i=0; i<num; i++)
				send_word(data[i]);
			mSerial.flush();
			return getack();
		}

		enum serial_result_t command_response(word c, word *data, size_t num, word *response) {
			enum serial_result_t r = command(c, data, num);
			if (r != SR_OK) return r;
			r = getword(response);
			return r;
		}

		enum serial_result_t open_file(const char *filename, char mode, word *handle) {
			empty_serial_buffer();
			send_word(0x000a);
			mSerial.write(filename, strlen(filename)+1);
			mSerial.write(mode);
			mSerial.flush();
			enum serial_result_t r = getack();
			if (r != SR_OK) return r;
			r = getword(handle);
			return r;
		}

		enum serial_result_t seek_file(word handle, word hi, word lo, word *status) {
			empty_serial_buffer()
			send_word(0xfe4f);
			send_word(handle);
			send_word(hi); send_word(lo);
			enum serial_result_t r = getack();
			if (r != SR_OK) return r;
			return getword(status);
		}

		void serial_printf(const char *format, ...) {
			va_list args;
			va_start(args, format);
			char buffer[128];
			vsprintf(buffer, format, args);
			va_end(args);
			Serial.print(buffer);
			Serial.flush();
		}
	};
}
