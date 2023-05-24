#ifndef KEYBOARDS_H
#define KEYBOARDS_H

#include <bits/stdint-intn.h>
#include <alsa/asoundlib.h>
#include "data/catalog.hpp"

enum Section { COMBI, GLOBAL, MULTI };
enum Page { PLAY = 0, TIMBRE };

#define NUMBER_OF_PARTS 8

class Keyboard {
public:
	Keyboard();
	void connect() noexcept;
	void disconnect() noexcept;
	void toggle_MIDI_state() noexcept;
	enum Switch get_MIDI_state() noexcept;
	bool is_connected() noexcept;
	void select_page( const enum Page & ) noexcept;
	void set_performance_buffer( const Performance &_Performance ) noexcept;
	auto get_performance_buffer() noexcept -> const Performance&;
	void to_prev_scene() noexcept;
	void to_next_scene() noexcept;
	void set_scene( const int16_t ) noexcept;
	void set_to_default_scene() noexcept;
	void dump_scene() noexcept;
	void dump_scene( const Performance &_Performance, const int16_t & ) noexcept;
	void set_name(const char *);
	void set_modality(const short);
	void dump_performance( const Performance &_Performance) noexcept;
	void set_song(const char);
private:
	Performance performance_buffer;
	int16_t scene;
	char name[20];
	bool activeMode;
	bool passiveMode;
	snd_rawmidi_t *device;
	char port[9];
	enum Switch MIDI;
};

#endif
