#pragma once

#include <Arduino.h>

typedef enum {
	C, D, E, F, G, A, B, C5,
} Note;

struct SongNote {
	Note note;
	unsigned long duration;
};

#define N_NOTES(song) sizeof(song)/sizeof(struct SongNote)

class Buzzer {
	public:
	Buzzer(int pin) : pin(pin) {
		pinMode(pin, OUTPUT);
	}

	void playNote(Note note, unsigned long duration) {
		int frequency = getFrequency(note);
		if (frequency < 0) return;

		tone(pin, frequency);
		delay(duration);
		noTone(pin);
		delay(50);
	}

	void playSong(struct SongNote *song, unsigned int numNotes) {
		for (int i=0; i<numNotes; i++) {
			playNote(song[i].note, song[i].duration);
		}
	}

	protected:
	int pin;

	int getFrequency(Note note) {
		switch(note) {
			case Note::C:
			return 262;

			case Note::D:
			return 294;

			case Note::E:
			return 330;

			case Note::F:
			return 349;

			case Note::G:
			return 392;

			case Note::A:
			return 440;

			case Note::B:
			return 494;

			case Note::C5:
			return 523;

			default:
			return -1;
		}
	}
};
