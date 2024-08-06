#ifndef KEYBOARDS_H
#define KEYBOARDS_H

#include <bits/stdint-intn.h>
#include <alsa/asoundlib.h>
#include <filesystem>
#include <jack/jack.h>
#include <jack/midiport.h>
#include <array>
#include <bits/stdint-intn.h>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>


#include "data/nlohmann/json.hpp"
#include "midi/midi.hpp"
#include "data/Catalog.hpp"
#include "utilities/timer.hpp"

static const size_t& PATCHES_PER_BANK {128};

static const size_t& PAGE_SYSEX_WORD_SIZE {7};
static const size_t& SCENE_SYSEX_PACK_SIZE {5};
static const size_t& NUMBER_OF_PARTS {8};
static const size_t& PARAM_SYSEX_WORD_SIZE {13};

struct Combination {
	std::vector<std::string> instruments;
};

enum Section { COMBI, GLOBAL, MULTI };
enum Page { PLAY = 0, TIMBRE };

class Keyboard {

public:
	Keyboard();
	Keyboard(const std::string& _Path);
	auto initialize() noexcept -> void;
	auto load_combs_from_json( const std::string & ) -> void;
	auto save_combs_to_json(const std::string& ) noexcept -> void;
	std::string get_instrument_name(const char &_Banco, const int16_t &_Num, const int16_t &_Track) noexcept;
	void set_instrument_name(const char &, const int16_t &, const int16_t &, const std::string_view) noexcept;
	size_t channels_per_combi;
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
	void dump_performance( const Performance &_Performance) noexcept;
	auto send_page_SysEx(jack_midi_data_t _SysEx[PAGE_SYSEX_WORD_SIZE]) -> void;
	auto send_scene_SysEx(jack_midi_data_t _SysEx[SCENE_SYSEX_PACK_SIZE][NUMBER_OF_PARTS][PARAM_SYSEX_WORD_SIZE]) -> void;
	auto send_PC(const jack_midi_data_t& _Bank, const jack_midi_data_t& _Program) noexcept -> void;
	auto write_sfz_file(const std::filesystem::path& _SFZfolder, const std::string& _TargetSFZ, const std::string& _OriginSFZ) const noexcept -> void;

private:
	std::vector<std::array<struct Combination, PATCHES_PER_BANK>> combinations;
	Performance performance_buffer;
	size_t n_bancos;
	int16_t scene;
	bool activeMode;
	bool passiveMode;
	enum Switch MIDI_state;
	Juanca::Timer timer;
};

// JSON
void from_json( const nlohmann::json &_JSONobject, struct Combination &_Combination );
void to_json(nlohmann::ordered_json& _J, const Combination& _C);

#endif
