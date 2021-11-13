#ifndef KEYBOARDS_H
#define KEYBOARDS_H

#define PLAY 	0
#define TIMBRE 	1

#include <alsa/asoundlib.h>

enum Page { COMBI, GLOBAL, MULTI };

class Keyboard {
public:
	Keyboard();
	void select_part(const short);
	void select_page( const enum Page );
	short part;
	short midiChannel;
	void set_name(const char *);
	void set_modality(const short);
	void set_program(const char, const short);
	void set_krome_program(const char, const short);
	void set_song(const char);
private:
	char name[20];
	snd_rawmidi_t *device;
	bool activeMode;
	bool passiveMode;
	char directory[50];
};

#endif
