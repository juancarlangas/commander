#include <bits/stdint-intn.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <jack/midiport.h>
#include <jack/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <ctime>
#include <iostream>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>

#include "midi/Keyboard.hpp"
#include "Catalog.hpp"

Performance performance_buffer;
std::int16_t current_scene;

int32_t value;

Keyboard::Keyboard(const std::string &_Path)/*{{{*/
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

	if (!combinations.empty()) {
		channels_per_combi = combinations[0][0].instruments.size();

		for (const auto &bank : combinations)
			for (const auto &combination : bank)
				if (combination.instruments.size() != channels_per_combi) {
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
    std::ofstream json_file{_Path};
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
	current_scene = performance_buffer.default_scene;
}/*}}}*/

auto Keyboard::get_performance_buffer() noexcept -> const Performance& {/*{{{*/
	return performance_buffer;
}/*}}}*/

void Keyboard::set_to_default_scene() noexcept/*{{{*/
{
	current_scene = performance_buffer.default_scene;
}/*}}}*/

void Keyboard::to_prev_scene() noexcept/*{{{*/
{
	if ( current_scene > 0 )
		--current_scene;
}/*}}}*/

void Keyboard::to_next_scene() noexcept/*{{{*/
{
	if ( current_scene < performance_buffer.n_scenes - 1 )
		++current_scene;
}/*}}}*/

void Keyboard::set_scene(const int16_t _Scene) noexcept/*{{{*/
{
	if (_Scene < performance_buffer.n_scenes)
		current_scene = _Scene;
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

/***************************** JSON **************************************/
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
std::array<jack_port_t*, 5> output_port {{NULL}};
std::array<void*, 5> output_buffer {{NULL}};
jack_port_t* input_port {NULL};
jack_nframes_t local_nframes = {0};/*}}}*/

// TARGET DATA{{{
const char** all_ports_C_String;/*}}}*/

// Control{{{
volatile bool should_send_PC {FALSE};
volatile bool should_send_page_SysEx {FALSE};
volatile bool should_send_scene_SysEx {FALSE};/*}}}*/

// CALLBACK MESSAGES{{{
struct PatchChangeT {
	jack_midi_data_t msb[3] {0xB0, 0x00, 0x00};
    jack_midi_data_t lsb[3] {0xB0, 0x20, 0x00};
    jack_midi_data_t pc[2]  {0xC0, 0x00};
} callback_PC;

//jack_midi_data_t callback_page_SysEx[PAGE_SYSEX_WORD_SIZE];
//jack_midi_data_t callback_scene_SysExEs[SCENE_SYSEX_PACK_SIZE][NUMBER_OF_PARTS][PARAM_SYSEX_WORD_SIZE];/*}}}*/

int process(jack_nframes_t nframes, [[maybe_unused]] void* arg) /*{{{*/
{
    // Buffer de entrada
    void* input_buffer = jack_port_get_buffer(input_port, nframes);
    jack_nframes_t event_count = jack_midi_get_event_count(input_buffer);
    jack_midi_event_t in_event;

    // Buffers de salida
    for (std::size_t n_port {0}; n_port < 5; ++n_port) {
        output_buffer[n_port] = jack_port_get_buffer(output_port[n_port], nframes);
        jack_midi_clear_buffer(output_buffer[n_port]);
    }

    // Enviar mensajes Program Change (PC)
    if (should_send_PC) {
        jack_midi_event_write(output_buffer[0], 0, callback_PC.msb, 
				sizeof(callback_PC.msb));
        jack_midi_event_write(output_buffer[0], 0, callback_PC.lsb, 
				sizeof(callback_PC.lsb));
        jack_midi_event_write(output_buffer[0], 0, callback_PC.pc, 
				sizeof(callback_PC.pc));
        should_send_PC = false;
    }

    // Procesar eventos MIDI
    for (jack_nframes_t i = 0; i < event_count; ++i) {
        if (jack_midi_event_get(&in_event, input_buffer, i) == 0) {

            // Si es un mensaje Control Change (CC) en canal 16,
			// reenviarlo al puerto 4
            if ((in_event.buffer[0] & 0xF0) == 0xB0 &&
				(in_event.buffer[0] & 0x0F) == 15) {
                jack_midi_event_write(output_buffer[4], in_event.time, 
						in_event.buffer, in_event.size);
                continue; // No procesar más este evento
            }

            const auto& strips {
				performance_buffer.scenes[current_scene].strips};

            // Reenviar cualquier tipo de evento MIDI
            const std::uint8_t& midi_event = in_event.buffer[0];
            const std::uint8_t& note_or_cc_or_pb = in_event.buffer[1];

            for (std::size_t j {0}; j < STRIPS_PER_PERFORMANCE; ++j) {
                if (strips[j].state == Switch::ON and
                    (midi_event == 0xB0 || midi_event == 0xE0 ||
                    (strips[j].lower_key <= note_or_cc_or_pb and
                    note_or_cc_or_pb <= strips[j].upper_key))) {

                    const std::uint8_t& strip_channel {
						static_cast<std::uint8_t>(strips[j].midi_ch)};

                    // Cambiamos el canal del evento
					// según el canal del strip
                    in_event.buffer[0] =
						(midi_event & 0xF0) | strip_channel;

                    // Si es una nota, ajustamos el transpose
                    if ((midi_event & 0xF0) == 0x90 ||
						(midi_event & 0xF0) == 0x80) {
                        in_event.buffer[1] += strips[j].transposition;
                    }

                    // Desactivar -Wpedantic para usar rangos en switch
                    #pragma GCC diagnostic push
                    #pragma GCC diagnostic ignored "-Wpedantic"
                    switch (j) {
                        case 0 ... 7:  // Strips 0-7
                            jack_midi_event_write(
									output_buffer[0], in_event.time, 
									in_event.buffer, in_event.size);
                            break;
                        case 8:  // Strip 8
                            jack_midi_event_write(
									output_buffer[1], in_event.time, 
									in_event.buffer, in_event.size);
                            break;
                        case 9:  // Strip 9
                            jack_midi_event_write(
									output_buffer[2], in_event.time, 
									in_event.buffer, in_event.size);
                            break;
                        case 10:  // Strip 10
                            jack_midi_event_write(
									output_buffer[3], in_event.time, 
									in_event.buffer, in_event.size);
                            break;
                    }
                    #pragma GCC diagnostic pop  // Restaurar advertencias

                }
            }
        }
    }

    return 0;
} /*}}}*/

bool isMidiPort(jack_port_t* port) {/*{{{*/
    const char* type = jack_port_type(port);
    return (strcmp(type, JACK_DEFAULT_MIDI_TYPE) == 0);
}/*}}}*/

void Keyboard::connect() noexcept {/*{{{*/
	// Connect to the JACK server
    if ((client = jack_client_open(client_name, JackNullOption, NULL)) ==
			NULL) {
		std::cerr << "Failed to open JACK client " << client_name <<
			" at Keyboard::connect()\n";
    	std::exit(EXIT_FAILURE); 
    }

	// Register the process callback
    jack_set_process_callback(client, process, 0);

    // Create the MIDI_state output port
	if ((output_port[0] =
			jack_port_register(client, "0_to_7",
				JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0)) == NULL) {
		std::cerr
			<< "Failed to register JACK port at Keyboard::connect()\n";
		std::exit(EXIT_FAILURE); 
	}

    // Create the MIDI_state output port
	if ((output_port[1] =
			jack_port_register(client, "8",
				JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0)) == NULL) {
		std::cerr
			<< "Failed to register JACK port at Keyboard::connect()\n";
		std::exit(EXIT_FAILURE); 
	}

    // Create the MIDI_state output port
	if ((output_port[2] =
			jack_port_register(client, "9",
				JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0)) == NULL) {
		std::cerr
			<< "Failed to register JACK port at Keyboard::connect()\n";
		std::exit(EXIT_FAILURE); 
	}

    // Create the MIDI_state output port
	if ((output_port[3] =
			jack_port_register(client, "10",
				JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0)) == NULL) {
		std::cerr
			<< "Failed to register JACK port at Keyboard::connect()\n";
		std::exit(EXIT_FAILURE); 
	}

    // Create the MIDI_state output port
	if ((output_port[4] =
			jack_port_register(client, "16",
				JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0)) == NULL) {
		std::cerr
			<< "Failed to register JACK port at Keyboard::connect()\n";
		std::exit(EXIT_FAILURE); 
	}

	// Create the MIDI input port
    if ((input_port = jack_port_register(client, "midi_in",
            JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0)) == NULL) {
        std::cerr << "Failed to register JACK input port at Keyboard::connect()\n";
        std::exit(EXIT_FAILURE); 
    }

    // Activate the client
    if (jack_activate(client)) {
		std::cerr << "Failed to activate JACK client "
			<< client_name << " at Keyboard::connect()\n";
    	std::exit(EXIT_FAILURE); 
    }

	// Get the available ports
    all_ports_C_String =
		jack_get_ports(client, NULL, NULL, JackPortIsInput);
	std::array<const char*, 5> desired_port_keyword {
		"a2j:Midi Through [14] (playback): [0] Midi Through Port-0",
		"CP-80:events-in",
		"Synth Pad:events-in",
		"Sampling:events-in",
		"LSP Mixer x4 Stereo:events-in"
	};

	for (std::int32_t i_port {0}; i_port < 5; ++i_port) {
		// Try each of them and connect to it
		for (int i = 0; all_ports_C_String[i] != NULL; ++i) {
			jack_port_t* possible_port = jack_port_by_name(client, all_ports_C_String[i]);
			if (possible_port != NULL && isMidiPort(possible_port)) {
				if (strstr(all_ports_C_String[i], desired_port_keyword[i_port]) != NULL) {
					if (jack_connect(client, jack_port_name(output_port[i_port]), all_ports_C_String[i]) != 0) {
						jack_free(all_ports_C_String);
						jack_client_close(client);
						std::exit(EXIT_FAILURE);
						break;
					}
				}
			}
		}
    }

    // Connect input port to desired input port (if applicable)
    // Here you should specify the desired port for MIDI input
    const char* desired_input_port_keyword {
		"a2j:Impact LX61  [24] (capture): [0] Impact LX61  MIDI1"
	};

    // Try each of them and connect to it
    all_ports_C_String = jack_get_ports(client, NULL, NULL, JackPortIsOutput);
    for (int i = 0; all_ports_C_String[i] != NULL; ++i) {
        jack_port_t* possible_port = jack_port_by_name(client, all_ports_C_String[i]);
        if (possible_port != NULL && isMidiPort(possible_port)) {
            if (strstr(all_ports_C_String[i], desired_input_port_keyword) != NULL) {
                if (jack_connect(client, all_ports_C_String[i], jack_port_name(input_port)) != 0) {
                    jack_free(all_ports_C_String);
                    jack_client_close(client);
                    std::exit(EXIT_FAILURE);
                }
            }
        }
    }

	MIDI_state = Switch::ON;
}
/*}}}*/

void Keyboard::dump_performance(const Performance& _Performance) noexcept {/*{{{*/
	// SET BUFFER
    set_performance_buffer(_Performance); // Pass to the buffer

	// PAGE
	/*
	jack_midi_data_t to_play_SysEx[] {
		0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x00, 0xF7};
	send_page_SysEx(to_play_SysEx);
	timer.sleep(2e8);
	*/

	// Patch Change
	send_PC(
		performance_buffer.program.bnk, performance_buffer.program.num);

	/*
	jack_midi_data_t to_edit_SysSex[] {
		0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x01, 0xF7};
	send_page_SysEx(to_edit_SysSex);
	timer.sleep(2e8);
	*/

	current_scene = 0;
	//dump_scene();
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
		if (performance_buffer.scenes[current_scene].strips[i].state == Switch::ON)
			param_SysExEs[0][i][11] = 0x00; // -> ON
											//
		param_SysExEs[1][i][11] = performance_buffer.scenes[current_scene].strips[i].volume;
		param_SysExEs[2][i][11] = performance_buffer.scenes[current_scene].strips[i].lower_key;
		param_SysExEs[3][i][11] = performance_buffer.scenes[current_scene].strips[i].upper_key;
		if ( performance_buffer.scenes[current_scene].strips[i].transposition < 0 ) {
			param_SysExEs[4][i][10] = 0x7F;
			param_SysExEs[4][i][11] = 0x80 + performance_buffer.scenes[current_scene].strips[i].transposition;
		}
		else
			param_SysExEs[4][i][11] = performance_buffer.scenes[current_scene].strips[i].transposition;
	}
	send_scene_SysEx(param_SysExEs);
}/*}}}*/

void Keyboard::dump_scene( const Performance &_Performance, const int16_t &_Scene ) noexcept/*{{{*/
{
	set_performance_buffer( _Performance );

	current_scene = _Scene;
	dump_scene();
}/*}}}*/

void Keyboard::disconnect() noexcept {/*{{{*/
    // Deactivate, unregister, and close the client
	jack_free(all_ports_C_String);	

    jack_deactivate(client);
	for (std::int32_t i {0}; i <= 3; ++i)
		jack_port_unregister(client, output_port[i]);
    jack_client_close(client);

    // Set MIDI_state switch to OFF or perform any necessary cleanup
    MIDI_state = Switch::OFF;
}/*}}}*/

auto Keyboard::send_page_SysEx(/*{{{*/
		[[maybe_unused]]jack_midi_data_t _SysEx[PAGE_SYSEX_WORD_SIZE]) -> void {
	//memcpy(callback_page_SysEx, _SysEx, PAGE_SYSEX_WORD_SIZE);{{{
	//should_send_page_SysEx = true;}}}
}/*}}}*/

auto Keyboard::send_scene_SysEx(/*{{{*/
		[[maybe_unused]]jack_midi_data_t _SysEx[SCENE_SYSEX_PACK_SIZE][NUMBER_OF_PARTS][PARAM_SYSEX_WORD_SIZE]) -> void {
	//memcpy(callback_scene_SysExEs, _SysEx, SCENE_SYSEX_PACK_SIZE * NUMBER_OF_PARTS * PARAM_SYSEX_WORD_SIZE);
	//should_send_scene_SysEx = true;
}/*}}}*/

auto Keyboard::send_PC(const jack_midi_data_t& _Bank, const jack_midi_data_t& _Program) noexcept -> void/*{{{*/
{
	callback_PC.lsb[2] = 32 + _Bank;
	callback_PC.pc[1] = _Program;

	should_send_PC = true;
}/*}}}*/
