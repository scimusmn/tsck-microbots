#pragma once

#include <SoftwareSerial.h>

#include <Diablo_Const4D.h>
#include <Diablo_Serial_4DLib.h>

namespace smm {
	class Screen {
		public:
		Screen(int rxPin, int txPin, int resetPin,
		       unsigned int width=800,
		       unsigned int height=480)
			: mSerial(rxPin, txPin),
			  mDisplay(((Stream*) &mSerial)),
			  mResetPin(resetPin),
			  mWidth(width), mHeight(height), mPosition(0),
			  mPrevious(-1)
		{}

		void setup()
		{
			mSerial.begin(9600);

			pinMode(mResetPin, OUTPUT);
			digitalWrite(mResetPin, 0);
			delay(100);
			digitalWrite(mResetPin, 1);
			delay(3000);

			mDisplay.gfx_ScreenMode(LANDSCAPE);
			mDisplay.gfx_Cls();

			mDisplay.putstr("display ready!");

			Serial.println("=== begin fs operations ===");

			const int max_retries = 15;
			int tries = 0;

			Serial.println(mSerial.available());
			while(mSerial.available()) {
				mSerial.read();
			}
			byte buf[] = { 0x00, 0x18, 0x48, 0x6c, 0x6c, 0x6f, 0x00 };
			mSerial.write(buf, sizeof(buf));
			while(mSerial.available() < 3);
			Serial.print("available: ");
			Serial.println(mSerial.available());
			while(mSerial.available()) {
				Serial.print("0x");
				Serial.println(mSerial.read(), HEX);
			}


			int err = mDisplay.file_Mount();
			while(tries < max_retries && err == 0) {
				Serial.println("mount failed, retrying...");
				err = mDisplay.file_Mount();
				tries += 1;
			}
			if (err == 0) {
				Serial.println("maximum attempts exceded. mount failed.");
				goto finish;
			}


			word handle = mDisplay.file_Open("img.gci", 'r');
			Serial.print("image handle: ");
			Serial.println(handle);
			err = mDisplay.file_Error();
			if (err != FE_OK) {
				Serial.print("failed to get file handle: ");
				Serial.println(err);
				goto finish;
			}
			mDisplay.file_Image(0, 0, handle);
			err = mDisplay.file_Error();
			if (err != FE_OK) {
				Serial.print("failed to get display image: ");
				Serial.println(err);
			}
finish:
			Serial.println("=== finish fs operations ===");
		}

		void push(double v)
		{
			int value = (int) (v * mHeight);

			if (mPrevious < 0)
				mPrevious = value;
			else {
				mDisplay.gfx_Line(mPosition, mPrevious,
				                  ++mPosition, value,
				                  GREEN);
				mPrevious = value;
				if (mPosition >= mWidth) {
					mPosition = 0;
					mDisplay.gfx_Cls();
				}
			}
		}

		private:
		SoftwareSerial mSerial;
		Diablo_Serial_4DLib mDisplay;
		int mResetPin;
		unsigned int mWidth, mHeight, mPosition;
		int mPrevious;
	};
}
