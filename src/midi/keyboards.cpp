#include <bits/stdint-intn.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <jack/midiport.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "midi/keyboards.hpp"
#include <ncurses.h>

#include <cstddef>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <stdexcept>
#include <string>
#include <algorithm>

#include "utilities/src/files.hpp"

int32_t value;

Keyboard::Keyboard( const std::string &_Path )/*{{{*/
{
	load_combs_from_json( _Path );
	MIDI = Switch::OFF;
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
	MIDI == Switch::OFF ? connect() : disconnect();
}/*}}}*/

enum Switch Keyboard::get_MIDI_state() noexcept/*{{{*/
{
	return MIDI;
}/*}}}*/

bool Keyboard::is_connected() noexcept/*{{{*/
{
	return MIDI == Switch::ON ? true : false;
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

/**************************************** JSON ***********************************************************/
void from_json( const nlohmann::json &_JSONobject, Combination &_Combination ) {/*{{{*/
    _JSONobject.at( "instruments" ).get_to( _Combination.instruments );
}/*}}}*/

void to_json(nlohmann::ordered_json& _J, const Combination& _C) {/*{{{*/
	_J = nlohmann::json{{"instruments", _C.instruments}};
}/*}}}*/

/****************************************** JACK *********************************************************/
// GENERIC MESSAGES{{{
jack_midi_data_t to_combi_SysEx[] {0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x00, 0xF7};
jack_midi_data_t to_timbre_SysEx[]{0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x01, 0xF7};
struct PatchChangeT {
	jack_midi_data_t msb[3] {0xB0, 0x00, 0x3F};
    jack_midi_data_t lsb[3] {0xB0, 0x20, 0x00};
    jack_midi_data_t pc[2]  {0xC0, 0x00};
} PatchChange;/*}}}*/

// Client {{{
const char* client_name {"Commander"};
jack_client_t* client {NULL};
jack_port_t* output_port {NULL};
jack_nframes_t local_nframes = {0};/*}}}*/

// Control{{{
volatile bool should_send_performance {FALSE};/*}}}*/

int process([[maybe_unused]]jack_nframes_t nframes, [[maybe_unused]]void* arg)/*{{{*/
{
	void* port_buffer = jack_port_get_buffer(output_port, nframes);
	jack_midi_clear_buffer(port_buffer);

    if (should_send_performance) {
		jack_midi_event_write(port_buffer, 0, to_combi_SysEx,  sizeof(to_combi_SysEx));
		jack_midi_event_write(port_buffer, 0, to_timbre_SysEx, sizeof(to_timbre_SysEx));
		jack_midi_event_write(port_buffer, 0, PatchChange.msb, sizeof(PatchChange.msb));
		jack_midi_event_write(port_buffer, 0, PatchChange.lsb, sizeof(PatchChange.lsb));
		jack_midi_event_write(port_buffer, 0, PatchChange.pc,  sizeof(PatchChange.pc));

		should_send_performance = false;
	}
    return 0;
}/*}}}*/

void Keyboard::connect() noexcept {/*{{{*/
	// Connect to the JACK server
    if ((client = jack_client_open(client_name, JackNullOption, NULL)) == NULL) {
		std::cerr << "Failed to open JACK client " << client_name << " at Keyboard::connect()\n";
    	std::exit(EXIT_FAILURE); 
    }

	// Register the process callback
    jack_set_process_callback(client, process, 0);

    // Create the MIDI output port
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

	MIDI = Switch::ON;
}
/*}}}*/

void Keyboard::dump_performance(const Performance& _Performance) noexcept {/*{{{*/
	// SET BUFFER
    set_performance_buffer(_Performance); // Pass to the buffer

    // SLEEP
    static struct timespec keyboardTimer;
    keyboardTimer.tv_sec = 0;
    keyboardTimer.tv_nsec = 200000000;

	// ADJUST MESSAGES
	PatchChange.lsb[2] = performance_buffer.patch.bnk - 65;
	PatchChange.pc[1] = performance_buffer.patch.num;

	should_send_performance = true;
}/*}}}*/

void Keyboard::disconnect() noexcept {/*{{{*/
    // Deactivate, unregister, and close the client
    jack_deactivate(client);
    jack_port_unregister(client, output_port);
    jack_client_close(client);

    // Set MIDI switch to OFF or perform any necessary cleanup
    MIDI = Switch::OFF;
}/*}}}*/

void Keyboard::dump_scene() noexcept/*{{{*/
{
	// sleep
	 static struct timespec keyboardTimer;
		keyboardTimer.tv_sec 	= 0;
		keyboardTimer.tv_nsec	= 200000000;
	

	// apagar sonidos
	//	unsigned char allSoundsOff[3] = {0xB0, 0x7B, 0x7F};

	// cambio de página
	//	unsigned char pageSysEx[2][7] =    {{0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x00, 0xF7},
	//										{0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x01, 0xF7}};
				
	///////////////////- Track: col 7 - control: col 9 - Value: col 11 /////////////////////////

	// Status (todos en OFF)
		unsigned char x50_on_off[8][13] = 
		{	{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x01, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x03, 0x00, 0x03, 0x00, 0x01, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x04, 0x00, 0x03, 0x00, 0x01, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x05, 0x00, 0x03, 0x00, 0x01, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x06, 0x00, 0x03, 0x00, 0x01, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x07, 0x00, 0x03, 0x00, 0x01, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x08, 0x00, 0x03, 0x00, 0x01, 0xF7}	};

	// Volúmen [X][11] es el volúmen
		[[ maybe_unused ]]unsigned char x50_volume[8][13] =
		{	{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x04, 0x00, 0x02, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x05, 0x00, 0x02, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x06, 0x00, 0x02, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x07, 0x00, 0x02, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x08, 0x00, 0x02, 0x00, 0x00, 0xF7}	};

	// Lower Key (C-1)
		unsigned char x50_lower_key[8][13] = 
		{	{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x01, 0x00, 0x22, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x02, 0x00, 0x22, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x03, 0x00, 0x22, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x04, 0x00, 0x22, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x05, 0x00, 0x22, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x06, 0x00, 0x22, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x07, 0x00, 0x22, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x08, 0x00, 0x22, 0x00, 0x00, 0xF7}	};

	// Upper Key (C-1)
		unsigned char x50_upper_key[8][13] = 
		{	{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x01, 0x00, 0x1F, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x02, 0x00, 0x1F, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x03, 0x00, 0x1F, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x04, 0x00, 0x1F, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x05, 0x00, 0x1F, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x06, 0x00, 0x1F, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x07, 0x00, 0x1F, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x08, 0x00, 0x1F, 0x00, 0x00, 0xF7}	};

	// Transposition (0)
		unsigned char x50_transposition[8][13] = 
		{	{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x01, 0x00, 0x0A, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x02, 0x00, 0x0A, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x03, 0x00, 0x0A, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x04, 0x00, 0x0A, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x05, 0x00, 0x0A, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x06, 0x00, 0x0A, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x07, 0x00, 0x0A, 0x00, 0x00, 0xF7},
			{0xF0, 0x42, 0x30, 0x7A, 0x41, 0x01, 0x00, 0x08, 0x00, 0x0A, 0x00, 0x00, 0xF7}	};

	// Ajuste
	for ( int16_t i = 0; i < 8; ++i ) {
		// STATE
		if ( performance_buffer.scenes[scene].tracks[i].state == ON )
			x50_on_off[i][11] = 0x00; // -> ON

		// volumen
		x50_volume[i][11] = performance_buffer.scenes[scene].tracks[i].volume;

		// zone
		x50_lower_key[i][11] = performance_buffer.scenes[scene].tracks[i].lower_key;
		x50_upper_key[i][11] = performance_buffer.scenes[scene].tracks[i].upper_key;

		// transposition =	columna 11 hacemos + porque es número negativo, de este modo
		// 					obtenemos una RESTA
		if ( performance_buffer.scenes[scene].tracks[i].transposition < 0 ) {
			x50_transposition[i][10] = 0x7F;
			x50_transposition[i][11] = 0x80 + performance_buffer.scenes[scene].tracks[i].transposition;
		}
		else
			x50_transposition[i][11] = performance_buffer.scenes[scene].tracks[i].transposition;
	}

	/*
	for ( int16_t channel = 0; channel < 8; ++channel ) {
		snd_rawmidi_write( device, x50_on_off[channel], 13 );
		snd_rawmidi_write( device, x50_volume[channel], 13 );
		snd_rawmidi_write( device, x50_lower_key[channel], 13);
		snd_rawmidi_write( device, x50_upper_key[channel], 13);
		snd_rawmidi_write( device, x50_transposition[channel], 13);
	}
	*/
}/*}}}*/

void Keyboard::dump_scene( const Performance &_Performance, const int16_t &_Scene ) noexcept/*{{{*/
{
	set_performance_buffer( _Performance );

	scene = _Scene;
	dump_scene();
}/*}}}*/

void Keyboard::select_page(const enum Page& _Pagina) noexcept {/*{{{*/
    // Sleep
    static struct timespec keyboardTimer;
    keyboardTimer.tv_sec = 0;
    keyboardTimer.tv_nsec = 200000000;

    // Change page
    const unsigned char pageSysEx[2][7] = {
        {0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x00, 0xF7},
        {0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x01, 0xF7}
    };

    jack_midi_data_t pageSysExBuffer[7];
    for (int i = 0; i < 7; ++i) {
        pageSysExBuffer[i] = pageSysEx[_Pagina][i];
    }

    // Create a JACK MIDI event
    jack_midi_event_t event;
    jack_midi_clear_buffer(event.buffer);
    jack_midi_data_t* buffer = jack_midi_event_reserve(event.buffer, 0, 7);
    memcpy(buffer, pageSysExBuffer, 7);

    // Write the MIDI event to the output port
    if (output_port != nullptr) {
        jack_midi_event_write(output_port, event.time, event.buffer, 7);
    }

    // Sleep
    nanosleep(&keyboardTimer, NULL);

    set_scene(performance_buffer.default_scene);
}/*}}}*/

void Keyboard::set_modality(short toMode) {/*{{{*/
    // Create the sysex message
    unsigned char sysex[7] = {0xF0, 0x42, 0x00, 0x7A, 0x4E, 0x00, 0xF7}; // Ch = sysex[2] (LSB)

    if (toMode == MULTI)
        sysex[5] = 0x04;

    // Create a JACK MIDI event
    jack_midi_data_t sysexBuffer[7];
    for (int i = 0; i < 7; ++i) {
        sysexBuffer[i] = sysex[i];
    }

    jack_midi_event_t event;
    jack_midi_clear_buffer(event.buffer);
    jack_midi_data_t* buffer = jack_midi_event_reserve(event.buffer, 0, 7);
    memcpy(buffer, sysexBuffer, 7);

    // Write the MIDI event to the output port
    if (output_port != nullptr) {
        jack_midi_event_write(output_port, event.time, event.buffer, 7);
    }
}/*}}}*/

