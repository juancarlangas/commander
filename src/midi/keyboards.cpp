#include <bits/stdint-intn.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <jack/midiport.h>
#include <jack/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <ctime>
#include "midi/keyboards.hpp"
#include "utilities/timer.hpp"

#include <cstddef>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>

int32_t value;

Keyboard::Keyboard( const std::string &_Path )/*{{{*/
{
	load_combs_from_json( _Path );
	MIDI_state = Switch::OFF;
}/*}}}*/

void Keyboard::load_combs_from_json( const std::string &_Path )/*{{{*/
{
	// LOAD DATA
	std::ifstream json_file{ _Path };
	if ( json_file.fail() ) {
		std::cerr << "Failed to open " + _Path + " in Keyboard::load_from_json()\n";
		exit(EXIT_FAILURE);
	}
	nlohmann::ordered_json json_object;
	json_file >> json_object;
	json_object.get_to(combinations);

	json_file.close();

	// CALCULATE SIZES
	n_bancos = combinations.size();

	if ( !combinations.empty() ) {
		channels_per_combi = combinations[0][0].instruments.size();

		for ( const auto &bank : combinations )
			for ( const auto &combination : bank )
				if ( combination.instruments.size() != channels_per_combi ) {
					std::cerr << "instruments vector size mismatch\n";
					exit( EXIT_FAILURE );
				}
	}
	else
		throw std::runtime_error( "combinations is empty" );
}/*}}}*/

auto Keyboard::save_combs_to_json(const std::string& _Path) noexcept -> void {/*{{{*/
	/*
	for (std::int32_t i = 0; i < 3; ++i) {
		combinations.push_back(std::array<Combination, 128> {});
		for (std::int32_t j = 0; j < 128; ++j) {
			for (std::int32_t k = 0; k < 8; ++k) {
				combinations[i][j].instruments.push_back("Piano");
			}
		}
	}
	*/

    // Create a JSON object and fill it with the data from combinations vector
    nlohmann::ordered_json json_object = combinations;

    // Open a file and write the JSON object to it
    std::ofstream json_file{ _Path };
    if (json_file.fail()) {
        std::cerr << "Failed to open " + _Path + " in Keyboards::save_combs_to_json()\n";
        exit(EXIT_FAILURE);
    }
    json_file << std::setfill('\t') << std::setw(1) << json_object << std::endl;
    json_file.close();
}/*}}}*/

std::string Keyboard::get_instrument_name(/*{{{*/
		const char &_Banco, const int16_t &_Numero, const int16_t &_Track ) noexcept
{
	return combinations[_Banco][_Numero].instruments[ _Track ];
}/*}}}*/

void Keyboard::set_instrument_name(/*{{{*/
		const char &_Banco, const int16_t &_Numero, const int16_t &_Pista,
		const std::string_view _Nombre )
			noexcept
{
	combinations[_Banco][_Numero].instruments[ _Pista ] = _Nombre;
}/*}}}*/

void Keyboard::toggle_MIDI_state() noexcept/*{{{*/
{
	MIDI_state == Switch::OFF ? connect() : disconnect();
}/*}}}*/

enum Switch Keyboard::get_MIDI_state() noexcept/*{{{*/
{
	return MIDI_state;
}/*}}}*/

bool Keyboard::is_connected() noexcept/*{{{*/
{
	return MIDI_state == Switch::ON ? true : false;
}/*}}}*/

void Keyboard::set_performance_buffer( const Performance &_Performance ) noexcept/*{{{*/
{
	performance_buffer = _Performance;
}/*}}}*/

auto Keyboard::get_performance_buffer() noexcept -> const Performance& {/*{{{*/
	return performance_buffer;
}/*}}}*/

void Keyboard::set_to_default_scene() noexcept/*{{{*/
{
	scene = performance_buffer.default_scene;
}/*}}}*/

void Keyboard::to_prev_scene() noexcept/*{{{*/
{
	if ( scene > 0 )
		--scene;
}/*}}}*/

void Keyboard::to_next_scene() noexcept/*{{{*/
{
	if ( scene < performance_buffer.n_scenes - 1 )
		++scene;
}/*}}}*/

void Keyboard::set_scene( const int16_t _Variacion ) noexcept/*{{{*/
{
	if ( _Variacion < performance_buffer.n_scenes )
		scene = _Variacion;
}/*}}}*/

auto Keyboard::write_sfz_file(const std::filesystem::path& _SFZfolder, const std::string& _TargetSFZ, const std::string& _OriginSFZ) const noexcept -> void {/*{{{*/
	std::ofstream sfz_file {_SFZfolder/_TargetSFZ};
	if (sfz_file.fail()) {
		std::cerr << _SFZfolder/_TargetSFZ << " could not be opened in Keyboard::write_sfz_file()" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	sfz_file << "<control>\n";
	sfz_file << "default_path=./\n";
	sfz_file << '\n';
	sfz_file << "#include \"" << _OriginSFZ << "\"";

	sfz_file.close();
}/*}}}*/

/**************************************** JSON ***********************************************************/
void from_json( const nlohmann::json &_JSONobject, Combination &_Combination ) {/*{{{*/
    _JSONobject.at( "instruments" ).get_to( _Combination.instruments );
}/*}}}*/

void to_json(nlohmann::ordered_json& _J, const Combination& _C) {/*{{{*/
	_J = nlohmann::json{{"instruments", _C.instruments}};
}/*}}}*/

/***************************** JACK **********************************/
// CLIENT DATA{{{
const char* client_name {"Commander"};
jack_client_t* client {NULL};
jack_port_t* output_port {NULL};
jack_nframes_t local_nframes = {0};/*}}}*/

// TARGET DATA{{{
const char** all_ports_C_String;/*}}}*/

// Control{{{
volatile bool should_send_PC {FALSE};
volatile bool should_send_page_SysEx {FALSE};
volatile bool should_send_scene_SysEx {FALSE};/*}}}*/

// CALLBACK MESSAGES{{{
struct PatchChangeT {
	jack_midi_data_t msb[3] {0xB0, 0x00, 0x3F};
    jack_midi_data_t lsb[3] {0xB0, 0x20, 0x00};
    jack_midi_data_t pc[2]  {0xC0, 0x00};
} callback_PC;

jack_midi_data_t callback_page_SysEx[PAGE_SYSEX_WORD_SIZE];
jack_midi_data_t callback_scene_SysExEs[SCENE_SYSEX_PACK_SIZE][NUMBER_OF_PARTS][PARAM_SYSEX_WORD_SIZE];/*}}}*/

int process([[maybe_unused]]jack_nframes_t nframes, [[maybe_unused]]void* arg)/*{{{*/
{
	void* port_buffer = jack_port_get_buffer(output_port, nframes);
	jack_midi_clear_buffer(port_buffer);

	if (should_send_PC) {
		jack_midi_event_write(port_buffer, 0, callback_PC.msb, sizeof(callback_PC.msb));
		jack_midi_event_write(port_buffer, 0, callback_PC.lsb, sizeof(callback_PC.lsb));
		jack_midi_event_write(port_buffer, 0, callback_PC.pc,  sizeof(callback_PC.pc));
		should_send_PC = false;
	}

	if (should_send_page_SysEx) {
		jack_midi_event_write(port_buffer, 0, callback_page_SysEx, PAGE_SYSEX_WORD_SIZE);
		should_send_page_SysEx = false;
	}
	
	if (should_send_scene_SysEx) {
		for (std::size_t i_param {0}; i_param < SCENE_SYSEX_PACK_SIZE; ++i_param)
			for (std::size_t i_part {0}; i_part < NUMBER_OF_PARTS; ++i_part)
		jack_midi_event_write(port_buffer, 0, callback_scene_SysExEs[i_param][i_part], PARAM_SYSEX_WORD_SIZE);
		should_send_scene_SysEx = false;
	}
	
    return 0;
}/*}}}*/

bool isMidiPort(jack_port_t* port) {/*{{{*/
    const char* type = jack_port_type(port);
    return (strcmp(type, JACK_DEFAULT_MIDI_TYPE) == 0);
}/*}}}*/

void Keyboard::connect() noexcept {/*{{{*/
	// Connect to the JACK server
    if ((client = jack_client_open(client_name, JackNullOption, NULL)) == NULL) {
		std::cerr << "Failed to open JACK client " << client_name << " at Keyboard::connect()\n";
    	std::exit(EXIT_FAILURE); 
    }

	// Register the process callback
    jack_set_process_callback(client, process, 0);

    // Create the MIDI_state output port
    if ((output_port = 
			jack_port_register(client, "midi_out", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0)) == NULL) {
		std::cerr << "Failed to register JACK port midi_out at Keyboard::connect()\n";
    	std::exit(EXIT_FAILURE); 
    }

    // Activate the client
    if (jack_activate(client)) {
		std::cerr << "Failed to activate JACK client " << client_name << " at Keyboard::connect()\n";
    	std::exit(EXIT_FAILURE); 
    }

	std::ofstream file {"/home/juancarlangas/Desktop/output.txt"};

	// Get the available ports
    all_ports_C_String = jack_get_ports(client, NULL, NULL, JackPortIsInput);
	const char* desired_port_keyword {"a2j:X50 [24] (playback): [0] X50 X50 _ SOUND"};

	// Try each of them and connect to it
    for (int i = 0; all_ports_C_String[i] != NULL; ++i) {
        jack_port_t* possible_port = jack_port_by_name(client, all_ports_C_String[i]);
        if (possible_port != NULL && isMidiPort(possible_port)) {
            if (strstr(all_ports_C_String[i], desired_port_keyword) != NULL) {
				if (jack_connect(client, jack_port_name(output_port), all_ports_C_String[i]) != 0) {
					file << "Failed to connect client to MIDI_state port." << std::endl;
					jack_free(all_ports_C_String);
					jack_client_close(client);
					std::exit(EXIT_FAILURE);
					break;
				}
			}
		}
    }

	file.close();
	MIDI_state = Switch::ON;
}
/*}}}*/

void Keyboard::dump_performance(const Performance& _Performance) noexcept {/*{{{*/
	// SET BUFFER
    set_performance_buffer(_Performance); // Pass to the buffer

	// PAGE
	jack_midi_data_t to_play_SysEx[] {0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x00, 0xF7};
	send_page_SysEx(to_play_SysEx);
	timer.sleep(2e8);

	// Patch Change
	send_PC(performance_buffer.patch.bnk - 65, performance_buffer.patch.num);

	jack_midi_data_t to_edit_SysSex[] {0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x01, 0xF7};
	send_page_SysEx(to_edit_SysSex);
	timer.sleep(2e8);

	scene = 0;
	dump_scene();
}/*}}}*/

void Keyboard::dump_scene() noexcept/*{{{*/
{
	jack_midi_data_t param_SysExEs[SCENE_SYSEX_PACK_SIZE][NUMBER_OF_PARTS][PARAM_SYSEX_WORD_SIZE]
		// Status (DEFAULT: todes en Off)
		{	{	{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x01, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x03, 0x00, 0x03, 0x00, 0x01, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x04, 0x00, 0x03, 0x00, 0x01, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x05, 0x00, 0x03, 0x00, 0x01, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x06, 0x00, 0x03, 0x00, 0x01, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x07, 0x00, 0x03, 0x00, 0x01, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x08, 0x00, 0x03, 0x00, 0x01, 0xF7}	},

			// Volúmen [X][11] es el volúmen
			{	{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x04, 0x00, 0x02, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x05, 0x00, 0x02, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x06, 0x00, 0x02, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x07, 0x00, 0x02, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x08, 0x00, 0x02, 0x00, 0x00, 0xF7}	},

			// Lower Key (C-1)
			{	{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x01, 0x00, 0x22, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x02, 0x00, 0x22, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x03, 0x00, 0x22, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x04, 0x00, 0x22, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x05, 0x00, 0x22, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x06, 0x00, 0x22, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x07, 0x00, 0x22, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x08, 0x00, 0x22, 0x00, 0x00, 0xF7}	},

			// Upper Key (C-1)
			{	{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x01, 0x00, 0x1F, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x02, 0x00, 0x1F, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x03, 0x00, 0x1F, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x04, 0x00, 0x1F, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x05, 0x00, 0x1F, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x06, 0x00, 0x1F, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x07, 0x00, 0x1F, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x08, 0x00, 0x1F, 0x00, 0x00, 0xF7}	},

			// Transposition (0)
			{	{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x01, 0x00, 0x0A, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x02, 0x00, 0x0A, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x03, 0x00, 0x0A, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x04, 0x00, 0x0A, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x05, 0x00, 0x0A, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x06, 0x00, 0x0A, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x07, 0x00, 0x0A, 0x00, 0x00, 0xF7},
				{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x08, 0x00, 0x0A, 0x00, 0x00, 0xF7}	}	};

	// ADJUST
	for ( std::size_t i = 0; i < NUMBER_OF_PARTS; ++i ) {
		if (performance_buffer.scenes[scene].tracks[i].state == State::INT)
			param_SysExEs[0][i][11] = 0x00; // -> ON
		else if (performance_buffer.scenes[scene].tracks[i].state == State::EXT)
			param_SysExEs[0][i][11] = 0x02; // -> EXT
											//
		param_SysExEs[1][i][11] = performance_buffer.scenes[scene].tracks[i].volume;
		param_SysExEs[2][i][11] = performance_buffer.scenes[scene].tracks[i].lower_key;
		param_SysExEs[3][i][11] = performance_buffer.scenes[scene].tracks[i].upper_key;
		if ( performance_buffer.scenes[scene].tracks[i].transposition < 0 ) {
			param_SysExEs[4][i][10] = 0x7F;
			param_SysExEs[4][i][11] = 0x80 + performance_buffer.scenes[scene].tracks[i].transposition;
		}
		else
			param_SysExEs[4][i][11] = performance_buffer.scenes[scene].tracks[i].transposition;
	}
	send_scene_SysEx(param_SysExEs);
}/*}}}*/

void Keyboard::dump_scene( const Performance &_Performance, const int16_t &_Scene ) noexcept/*{{{*/
{
	set_performance_buffer( _Performance );

	scene = _Scene;
	dump_scene();
}/*}}}*/

void Keyboard::disconnect() noexcept {/*{{{*/
    // Deactivate, unregister, and close the client
	jack_free(all_ports_C_String);	

    jack_deactivate(client);
    jack_port_unregister(client, output_port);
    jack_client_close(client);

    // Set MIDI_state switch to OFF or perform any necessary cleanup
    MIDI_state = Switch::OFF;
}/*}}}*/

auto Keyboard::send_page_SysEx(jack_midi_data_t _SysEx[PAGE_SYSEX_WORD_SIZE]) -> void {/*{{{*/
	memcpy(callback_page_SysEx, _SysEx, PAGE_SYSEX_WORD_SIZE);
	should_send_page_SysEx = true;
}/*}}}*/

auto Keyboard::send_scene_SysEx(jack_midi_data_t _SysEx[SCENE_SYSEX_PACK_SIZE][NUMBER_OF_PARTS][PARAM_SYSEX_WORD_SIZE]) -> void {/*{{{*/
	memcpy(callback_scene_SysExEs, _SysEx, SCENE_SYSEX_PACK_SIZE * NUMBER_OF_PARTS * PARAM_SYSEX_WORD_SIZE);

	should_send_scene_SysEx = true;
}/*}}}*/

auto Keyboard::send_PC(const jack_midi_data_t& _Bank, const jack_midi_data_t& _Program) noexcept -> void/*{{{*/
{
	callback_PC.lsb[2] = _Bank;
	callback_PC.pc[1] = _Program;

	should_send_PC = true;
}/*}}}*/

