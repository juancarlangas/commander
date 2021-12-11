#ifndef KEYBOARDS_H
#define KEYBOARDS_H

#include <bits/stdint-intn.h>
#include <alsa/asoundlib.h>
#include "data/databases.hpp"

enum Section { COMBI, GLOBAL, MULTI };
enum Page { PLAY = 0, TIMBRE };

class Keyboard {
public:
	Keyboard();
	void connect() noexcept;
	void disconnect() noexcept;
	void toggle_MIDI_state() noexcept;
	void set_buffer( const struct System & ) noexcept;
	void prev_variation() noexcept;
	void next_variation() noexcept;
	void set_variation( const int16_t ) noexcept;
	void reset_variation() noexcept;
	void dump_variation() noexcept;
	void dump_variation( const struct System &, const int16_t & ) noexcept;
	void select_page( const enum Page & ) noexcept;
	void select_part(const short);
	void select_section( const enum Section ) noexcept;
	short part;
	short midiChannel;
	void set_name(const char *);
	void set_modality(const short);
	void set_program( const struct System &_Buffer ) noexcept;
	void set_krome_program(const char, const short);
	void set_song(const char);
	enum Switch get_MIDI_state() noexcept;
	bool is_connected() noexcept;
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

	enum Switch MIDI;
};

#endif
