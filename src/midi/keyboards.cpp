#include <bits/stdint-intn.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "midi/keyboards.hpp"
#include <ncurses.h>

#include <cstddef>
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

auto Keyboard::process([[maybe_unused]]jack_nframes_t nframes, [[maybe_unused]]void* arg) -> int {/*{{{*/
	return 0;
}/*}}}*/

auto Keyboard::jackShutdown() -> void {}

void Keyboard::connect() noexcept/*{{{*/
{
	client = jack_client_open(client_name, options, &status, server_name);
	if (client == NULL) {
		std::cerr << "No se pudo abrir " << client_name << " en Keyboard::connect() ALV\n";
		std::exit(EXIT_FAILURE);
	}

	// Activate the JACK client
	if (jack_activate(client)) {
		std::cerr << "No se pudo activar " << client_name << " en Keyboard::connect() ALV\n";
		std::exit(EXIT_FAILURE);
	}

	// Register the process callback function
	jack_set_process_callback(client, [](jack_nframes_t nframes, void* arg) -> int {
    Keyboard* handler = static_cast<Keyboard*>(arg);

    // Retrieve the input and output MIDI buffers
    [[maybe_unused]]void* inputBuffer = jack_port_get_buffer(handler->inputPort, nframes);
    [[maybe_unused]]void* outputBuffer = jack_port_get_buffer(handler->outputPort, nframes);

    // Process MIDI data
    // Example: Simply copy the input MIDI data to the output MIDI data
    // jack_midi_copy_buffer(outputBuffer, inputBuffer);

    return 0;
}, this);

	// Register the shutdown callback function
	jack_on_shutdown(client, [](void* arg) {
		Keyboard* handler = static_cast<Keyboard*>(arg);
		handler->jackShutdown();
	}, this);

	// Register the input/output ports
	jack_port_t* inputPort = jack_port_register(client, "input", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
	jack_port_t* outputPort = jack_port_register(client, "output", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);
    if (inputPort == NULL || outputPort == NULL) {
        std::cerr << "Failed to register input/output ports in Keyboard::connect()\n";
        std::exit(EXIT_FAILURE);
    }

	MIDI = Switch::ON;
}/*}}}*/

void Keyboard::disconnect() noexcept {/*{{{*/
	// Disconnect input port
    if (inputPort != nullptr) {
        jack_port_disconnect(client, inputPort);
        inputPort = nullptr;
    }

    // Disconnect output port
    if (outputPort != nullptr) {
        jack_port_disconnect(client, outputPort);
        outputPort = nullptr;
    }

    // Close the JACK client
    jack_client_close(client);

    // Set MIDI switch to OFF or perform any necessary cleanup
    MIDI = Switch::OFF;
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

void Keyboard::dump_scene() noexcept/*{{{*/
{
	// sleep
	/*
	 * static struct timespec keyboardTimer;
		keyboardTimer.tv_sec 	= 0;
		keyboardTimer.tv_nsec	= 200000000;
	*/

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

	for ( int16_t channel = 0; channel < 8; ++channel ) {
		snd_rawmidi_write( device, x50_on_off[channel], 13 );
		snd_rawmidi_write( device, x50_volume[channel], 13 );
		snd_rawmidi_write( device, x50_lower_key[channel], 13);
		snd_rawmidi_write( device, x50_upper_key[channel], 13);
		snd_rawmidi_write( device, x50_transposition[channel], 13);
	}
}/*}}}*/

void Keyboard::dump_scene( const Performance &_Performance, const int16_t &_Variacion ) noexcept/*{{{*/
{
	set_performance_buffer( _Performance );
	scene = _Variacion;
	dump_scene();
}/*}}}*/

void Keyboard::set_name(const char *id)/*{{{*/
{
	sprintf(name, "%s", id);
}/*}}}*/

void Keyboard::select_page( const enum Page &_Pagina ) noexcept/*{{{*/
{
	// sleep
	static struct timespec keyboardTimer;
		keyboardTimer.tv_sec 	= 0;
		keyboardTimer.tv_nsec	= 200000000;

	// cambio de página
	unsigned char pageSysEx[2][7] =    {{0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x00, 0xF7},
										{0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x01, 0xF7}};

	snd_rawmidi_write( device, pageSysEx[ _Pagina ], 7 );
	nanosleep( &keyboardTimer, NULL );

	set_scene( performance_buffer.default_scene );
}/*}}}*/

void Keyboard::set_modality(short toMode)/*{{{*/
{
	snd_rawmidi_t *device = NULL;
	unsigned char sysex[7] = {0xF0, 0x42, 0x00, 0x7A, 0x4E, 0x00, 0xF7}; //Ch = sysex[2] (LSB)

	if (toMode == MULTI)
		sysex[5] = 0x04;

	
	snd_rawmidi_write(device, sysex, 7);
}/*}}}*/

void Keyboard::dump_performance( const Performance &_Performance ) noexcept/*{{{*/
{
	set_performance_buffer( _Performance ); // We pass to the buffer

	// sleep
	static struct timespec keyboardTimer;
		keyboardTimer.tv_sec 	= 0;
		keyboardTimer.tv_nsec	= 200000000;

	// cambio de página
	unsigned char pageSysEx[2][7] =    {{0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x00, 0xF7},
										{0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x01, 0xF7}};

	// Midi Ch = msb[0], lsb[0], pc[0] igual, solo el ultimo digito
	unsigned char msb[3] = {0xB0, 0x00, 0x3F}, lsb[3] = {0xB0, 0x20, 0x00}, pc[2] = {0xC0, 0x00};

	// ajuste
	lsb[2] = performance_buffer.patch.bnk - 65; /*LSB*/
	pc[1] = performance_buffer.patch.num; /*PC*/

	snd_rawmidi_write( device, pageSysEx[COMBI], 7 );
	nanosleep( &keyboardTimer, NULL );
		snd_rawmidi_write(device, msb, 3);
		snd_rawmidi_write(device, lsb, 3);
		snd_rawmidi_write(device, pc, 2);
	snd_rawmidi_write( device, pageSysEx[TIMBRE], 7 );
	nanosleep( &keyboardTimer, NULL );

	set_scene( performance_buffer.default_scene );
	dump_scene();
}/*}}}*/

void Keyboard::set_song(const char song)/*{{{*/
{
	snd_rawmidi_t *device = NULL;
	unsigned char songSelect[2] = {0xF3, 0x00};

	
	songSelect[1] = song; /*SongSelect */
		snd_rawmidi_write(device, songSelect, 2);	
}/*}}}*/

void from_json( const nlohmann::json &_JSONobject, Combination &_Combination ) {/*{{{*/
    _JSONobject.at( "instruments" ).get_to( _Combination.instruments );
}/*}}}*/

void to_json(nlohmann::ordered_json& _J, const Combination& _C) {/*{{{*/
	_J = nlohmann::json{{"instruments", _C.instruments}};
}/*}}}*/
