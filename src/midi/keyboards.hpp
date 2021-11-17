#ifndef KEYBOARDS_H
#define KEYBOARDS_H

#define PLAY 	0
#define TIMBRE 	1

#include <alsa/asoundlib.h>
#include "data/databases.hpp"

enum Page { COMBI, GLOBAL, MULTI };

class Keyboard {
public:
	Keyboard();
	void set_buffer( const struct System & ) noexcept;
	void set_variation( const int16_t ) noexcept;
	void prev_variation() noexcept;
	void next_variation() noexcept;
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
	struct System buffer;
	int16_t variacion;
	char name[20];
	bool activeMode;
	bool passiveMode;
	char directory[50];

	snd_rawmidi_t *device;
	char port[9];

	unsigned char allSoundsOff[3];
};

#endif
