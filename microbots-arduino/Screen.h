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
			  mResetPin(resetPin),
			  mWidth(width), mHeight(height), mPosition(0),
			  mPrevious(-1)
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
			byte landscape[] = {0, 0};
			r = command_response(0xff, 0x42, landscape, 2, NULL);
			if (r != SR_OK)
				serial_printf("screen mode failed: %d\n", r);
			else
				serial_printf("set screen to landscape\n");

			/* clear screen */
			r = command(0xff, 0x82, NULL, 0);
			if (r != SR_OK)
				serial_printf("clear screen failed: %d\n", r);
			else
				serial_printf("cleared screen\n");

			/* mount FAT16 fs */
			word success;
			r = command_response(0xfe, 0x3c, NULL, 0, &success);
			if (r != SR_OK)
				serial_printf("mount fs failed: %d\n", r);
			else if (success == 0)
				serial_printf("mount fs failed!\n");
			else
				serial_printf("mounted fs\n");


			/* open image file */
			const char * fname = "microb~1.gci\0r";
			word handle;
			r = command_response(0x00, 0x0a, (byte*) fname, 14, &handle);
			if (r != SR_OK)
				serial_printf("open '%s' failed: %d\n", fname, r);
			else if (handle == 0)
				serial_printf("could not find file '%s'\n", fname);
			else
				serial_printf("opened '%s'\n", fname);


			/* display image */
			word error;
			byte xy_handle[] = {
				0, 0, /* x pos */
				0, 0, /* y pos */
				handle >> 8, handle,
			};
			r = command_response(0xfe, 0x4a, xy_handle, 6, &error);
			if (r != SR_OK)
				serial_printf("display image failed: %d\n", r);
			else if (error != 0)
				serial_printf("display image failed due to file error: %d\n");
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

		enum serial_result_t command(byte hi, byte lo, byte *data, size_t num) {
			empty_serial_buffer();
			mSerial.write(hi);
			mSerial.write(lo);
			for (int i=0; i<num; i++)
				mSerial.write(data[i]);
			mSerial.flush();
			return getack();
		}

		enum serial_result_t command_response(byte hi, byte lo, byte *data, size_t num, word *response) {
			enum serial_result_t r = command(hi, lo, data, num);
			if (r != SR_OK) return r;
			r = getword(response);
			return r;
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
