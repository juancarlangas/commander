#include "Orchestra.hpp"
#include "elements/windows/popups/orchestra_elements/text_popup.hpp"
#include "elements/windows/popups/orchestra_elements/double_X_slider.hpp"
#include "midi/midi.hpp"
#include "ui/colors.hpp"
#include "ui/elements/window.hpp"
#include "ui/ncurses.hpp"
#include "midi/Keyboard.hpp"
#include "printing.hpp"
#include "utils/timer.hpp"
#include <ncurses.h>
#include <panel.h>
#include <string>

using Coordinates::X;
using Coordinates::Y;

Juanca::Timer timer;

Orchestra::Orchestra() : // Sets colors /*{{{*/
	native_font ({	{GREEN_DEFAULT, "Bold"},
					{BLUE_DEFAULT, "Bold"},
					{YELLOW_DEFAULT, "Bold"},
					{MAGENTA_DEFAULT, "Bold"},
					{CYAN_DEFAULT, "Bold"},
					{RED_DEFAULT, "Bold"},
					{YELLOW_DEFAULT, "Regular"},
					{CYAN_DEFAULT, "Regular"},
					{GREEN_DEFAULT, "Bold"},
					{BLUE_DEFAULT, "Bold"},
					{YELLOW_DEFAULT, "Bold"},
					{MAGENTA_DEFAULT, "Bold"},
					{CYAN_DEFAULT, "Bold"},
					{RED_DEFAULT, "Bold"},
					{YELLOW_DEFAULT, "Regular"},
					{CYAN_DEFAULT, "Regular"}}),
	cursor_font {WHITE_DEFAULT, "Bold"},
	MIDI_font {GREEN_DEFAULT, "Bold"},
	dimmed_font {GRAY_DEFAULT, "Bold"},
	light_font {WHITE_DEFAULT, "Bold"}
{}/*}}}*/

void Orchestra::link_MIDI_device( Keyboard *_Teclado ) noexcept/*{{{*/
{
	keyboard_ptr = _Teclado;
}/*}}}*/

void Orchestra::init( const int32_t _Ysize, const int32_t _Xsize,/*{{{*/
						const int32_t _Ypos, const int32_t _Xpos ) noexcept
{
	current_scene = 0;

	// Base
	base.init( _Ysize, _Xsize, _Ypos, _Xpos, 1 );
	base.set_box_color(WHITE_DEFAULT);
	base.set_box_width( "Bold" );
	base.define_box( 0, 0, 0, 0, 0, 0, 0, 0 );
	base.update();

	// Indicador de variación
	scene_text_box.Popup::init(3, 20, _Ypos + 1, _Xpos + 120);
	scene_text_box.set_font_color( GRAY_DEFAULT );
	scene_text_box.set_font_width( "Bold" );
	scene_text_box.update();

	// keyboard_ptr_text_box
	keyboard_scheme.Popup::init(
			5, 61, _Ypos + (_Ysize * 40 / 200), _Xpos + (_Xsize * 66 / 200));
	keyboard_scheme.set_font_color(WHITE_DEFAULT);
	keyboard_scheme.set_font_width("Bold");
	keyboard_scheme.auto_draw();
	keyboard_scheme.update();

	// Variación inicial
	vi_field.create("VI", 3, 6, _Ypos + 1, _Xpos + 2, STRING, 16);
	vi_field.set_font_width("Bold");
	vi_field.set_font_color(WHITE_DEFAULT);

	// Etiqueta
	etiqueta_field.create("Etiqueta", 3, _Xsize * 80 / 200, _Ypos + 1, _Xpos + (_Xsize / 4),
							STRING, 16);
	etiqueta_field.set_font_width("Bold");
	etiqueta_field.set_font_color(WHITE_DEFAULT);

	int32_t y_starting_point = _Ypos + (_Ysize * 70 / 200);

	for (std::size_t i {0}; i < NUMBER_OF_PARTS; ++i) {
		// Since channel 1 intended to be "General".. Just temp fixes
		Font specific_font {i == 0 ? light_font : native_font[i]};

		status_field[i].init(y_starting_point + i, _Xpos + 2,
				specific_font, dimmed_font, cursor_font, light_font,
				i + 49 /*ASCII*/);
		status_field[i].update();

		channel_field[i].init(1, 4, y_starting_point + i, _Xpos + 5,
				specific_font, dimmed_font, cursor_font, light_font);
		transposition_field[i].update();

		instrument_field[i].init(1, 28, y_starting_point + i, _Xpos + 9,
				specific_font, dimmed_font, cursor_font, light_font);
		instrument_field[i].update();

		volume_field[i].init(1, 4, y_starting_point + i, _Xpos + 35,
				specific_font, dimmed_font, cursor_font, light_font);
		volume_field[i].update();

		transposition_field[i].init( 1, 4, y_starting_point + i, _Xpos + 45,
				specific_font, dimmed_font, cursor_font, light_font);
		transposition_field[i].update();
		
		// invocamos su clase base porque no cuadra con su init
		double_X_slider[i].OrchestraElement::init( 1, 62, y_starting_point + i, _Xpos + 49,
				specific_font, dimmed_font, cursor_font, light_font);
		double_X_slider[i].update();
	}

	hide();
}/*}}}*/

void KeyboardScheme::auto_draw() noexcept/*{{{*/

{
	int32_t key, pixel, nota;
	for ( pixel = 0; pixel < 3; ++pixel )
		for ( key = 0; key < 61; ++key ) {
			nota = key % 12;
			if ( ( pixel == 0 or pixel == 1 ) and
					( nota == Notas::C_SUS or
					  nota == Notas::D_SUS or
					  nota == Notas::F_SUS or
					  nota == Notas::G_SUS or
					  nota == Notas::A_SUS ) )
				wattroff( area, A_REVERSE );
			else
				wattron( area, A_REVERSE );
			mvwaddch( area, pixel, key, ' ' );
		}
}/*}}}*/

void Orchestra::update() noexcept/*{{{*/ {
	/* This method actually is the responsable of updating the fields with the true values */

	scene_text_box.set_text("Variacion " + std::to_string(current_scene + 1) +
								" de " + std::to_string(info->n_scenes));
	vi_field.set_content(std::to_string( info->default_scene));
	etiqueta_field.set_content(info->scenes[current_scene].label);

	for (std::size_t i {0}; i < keyboard_ptr->channels_per_combi; ++i) {

		// Preparando las dobles barras
		int16_t &l_Value = info->scenes[current_scene].strips[i].lower_key;
		int16_t &r_Value = info->scenes[current_scene].strips[i].upper_key;

		// Límites
		if (r_Value < MIN_KEY)
			r_Value = MIN_KEY;
		if (l_Value < MIN_KEY)
			l_Value = MIN_KEY;
		if (l_Value > MAX_KEY)
			l_Value = MAX_KEY;
		if (r_Value > MAX_KEY)
			r_Value = MAX_KEY;
		// cruzamientos
		if (l_Value > r_Value)
			r_Value = l_Value;
		double_X_slider[i].set_values(l_Value, r_Value);

		if (info->scenes[current_scene].strips[i].state == Switch::ON) {
			status_field[i].on();
			channel_field[i].on();
			instrument_field[i].on();
			volume_field[i].on();
			transposition_field[i].on();
			double_X_slider[i].on();
		}
		else {
			status_field[i].off();
			channel_field[i].off();
			instrument_field[i].off();
			volume_field[i].off();
			transposition_field[i].off();
			double_X_slider[i].off();
		}

		instrument_field[i].set_text(keyboard_ptr->get_instrument_name(info->program.bnk, info->program.num,i));
		channel_field[i].set_text(
				std::to_string(info->scenes[current_scene].strips[i].midi_ch));
		volume_field[i].set_text(std::to_string( info->scenes[ current_scene ].strips[ i ].volume ) );
		transposition_field[i].set_text(
				std::to_string(info->scenes[current_scene].strips[i].transposition));
	}
}/*}}}*/

void Orchestra::show( Performance *&_Performance ) noexcept/*{{{*/
{
	// solo obtiene la información y aparece los páneles
	info = _Performance;

	base.show();
	scene_text_box.show();
	keyboard_scheme.show();
	vi_field.show();
	etiqueta_field.show();

	for ( int32_t i = 0; i < 16; ++i ) {
		status_field[i].show();
		channel_field[i].show();
		instrument_field[i].show();
		volume_field[i].show();
		transposition_field[i].show();
		double_X_slider[i].show();
	}

	update();
}/*}}}*/

void Orchestra::hide() noexcept/*{{{*/
{	
	for ( int32_t i = 0; i < 16; ++i ) {
		status_field[i].hide();
		channel_field[i].hide();
		instrument_field[i].hide();
		volume_field[i].hide();
		transposition_field[i].hide();
		double_X_slider[i].hide();
	}

	vi_field.hide();
	etiqueta_field.hide();
	scene_text_box.hide();
	keyboard_scheme.hide();
	base.hide();
}/*}}}*/

/* Las siguientes funciones usualmente solo mueven una cosita, pero por esa cosita
 * se pierde el cursor donde debe de ir, por eso es que se hace todo eso.
 *
 * cursor[2] representa los valores X y Y de la malla del cursor virtual ( 7 x 9 ).
 * y = -1 representa la label de hasta arriba; en este caso, dicha label siempre tendrá
 * el cursor independientemente del valor de X */

void Orchestra::capture_key() noexcept/*{{{*/
{
	/* Esta función es un bucle que comienza cuando aparece la ventana
	 * y termina hasta que se cierra */
	cursor[Coordinates::X] = 0;
	cursor[Coordinates::Y] = -1;

	curs_set(true);
	vi_field.set_cursor(); // Por defalt el cursor estará sobre el campo default_scene

	/* Cadena para almacenar la palabra la palabra que está siendo editada. */
	temp_word = std::to_string(info->default_scene);
	int32_t tecla;
	char tecla_c_string[3];
	bool again = true;
	bool will_dump;

	do {
		switch (tecla = getch()) {
			case 11: // <C-k> : TOGGLE_MIDI_STATE{{{
				keyboard_ptr->toggle_MIDI_state();
				print_MIDI_state(MIDI_state_window, keyboard_ptr->get_MIDI_state());

				if (cursor[Y] == -1) {// si está hasta arriba
					cursor[X] == CursorX::DEFAULT_SCENE ? vi_field.set_cursor() : etiqueta_field.set_cursor();
				}
				else {
					switch (cursor[X]) {
						case CursorX::STATE :
							status_field[cursor[Y]].set_cursor();
							break;
						case CursorX::MIDI_CH :
							channel_field[cursor[Y]].set_cursor();
							break;
						case CursorX::INSTRUMENT:
							instrument_field[cursor[Y]].set_cursor();
							break;
						case CursorX::VOLUME :
							volume_field[ cursor[Y] ].set_cursor();
							break;
						case CursorX::TRANSPOSITION :
							transposition_field[cursor[Y]].set_cursor();
							break;
						case CursorX::L_SLIDER:
							double_X_slider[cursor[Coordinates::Y]].set_cursor_at_left();
							break;
						case CursorX::R_SLIDER:
							double_X_slider[cursor[Coordinates::Y]].set_cursor_at_right();
							break;
					}
				}
				will_dump = false;

				break;/*}}}*/

			case 353 : // <S-TAB> : Variación anterior{{{
				if (current_scene > 0) { // si se puede ir atrás
					--current_scene;
					update();

					if (cursor[Y] == -1) { // si está hasta arriba
						if (cursor[X] == CursorX::DEFAULT_SCENE) {
							vi_field.set_content(std::to_string(info->default_scene));
							info->default_scene = std::stoi(temp_word);
							vi_field.set_cursor();
						}
						else {
							info->scenes[current_scene + 1].label = temp_word; // guardamos
							temp_word = info->scenes[current_scene].label;
							etiqueta_field.set_cursor();
						}
					}
					else
						switch (cursor[X]) {
							case CursorX::STATE :
								status_field[cursor[Y]].set_cursor();
								break;
							case CursorX::MIDI_CH :
								info->scenes[current_scene + 1].strips[cursor[Y]].midi_ch = std::stoi(temp_word);
								temp_word = 
									info->scenes[current_scene].strips[cursor[Y]].midi_ch == 0
									? "0" : std::to_string(
									info->scenes[current_scene].strips[cursor[Y]].midi_ch);
								channel_field[cursor[Y]].set_cursor();
								break;
							case CursorX::INSTRUMENT :
								keyboard_ptr->set_instrument_name(info->program.bnk, info->program.num, cursor[Y], temp_word);
								instrument_field[cursor[Y]].set_cursor();
								break;
							case CursorX::VOLUME:
								info->scenes[current_scene + 1].strips[cursor[Y]].volume = std::stoi( temp_word );
								temp_word = info->scenes[current_scene].strips[cursor[Y]].volume == 0 ?
									"0" : std::to_string(info->scenes[current_scene].strips[cursor[Y]].volume);
								volume_field[cursor[Y]].set_cursor();
								break;
							case CursorX::TRANSPOSITION :
								info->scenes[current_scene + 1].strips[cursor[Y]].transposition = std::stoi( temp_word );
								temp_word = 
									info->scenes[current_scene].strips[cursor[Y]].transposition == 0
									? "0" : std::to_string(
									info->scenes[current_scene].strips[cursor[Y]].transposition );
								transposition_field[cursor[Y] ].set_cursor();
								break;
							case CursorX::L_SLIDER:
								double_X_slider[cursor[Coordinates::Y]].set_cursor_at_left();
								break;
							case CursorX::R_SLIDER:
								double_X_slider[cursor[Coordinates::Y]].set_cursor_at_right();
								break;
						}

					if (keyboard_ptr->get_MIDI_state() == Switch::ON) {
						// TO EDIT
						//jack_midi_data_t to_edit_SysEx[] {0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x01, 0xF7};
						//keyboard_ptr->send_page_SysEx(to_edit_SysEx);
						//timer.sleep(2e8);
	
						//keyboard_ptr->dump_scene( *info, current_scene );
					}
					will_dump = false;
				}
				break;/*}}}*/

			case 9 : // <TAB> : Variación siguiente{{{
				if ( current_scene < info->n_scenes - 1 ) {
					++current_scene;
					update();
					if (cursor[Y] == -1) { // si está hasta arriba
						if (cursor[Coordinates::X] == CursorX::DEFAULT_SCENE) {
							info->default_scene = std::stoi(temp_word);
							vi_field.set_content( std::to_string(info->default_scene));
							vi_field.set_cursor();
						}
						else {
							info->scenes[current_scene - 1].label = temp_word; // guardamos
							temp_word = info->scenes[current_scene].label; // copiamos nuevo
							etiqueta_field.set_cursor();
						}
					}
					else
						switch (cursor[Coordinates::X]) {
							case CursorX::STATE:
								status_field[cursor[Coordinates::Y]].set_cursor();
								break;
							case CursorX::INSTRUMENT:
								keyboard_ptr->set_instrument_name(info->program.bnk, info->program.num, cursor[Y], temp_word);
								instrument_field[cursor[Coordinates::Y]].set_cursor();
								break;
							case CursorX::MIDI_CH:
								info->scenes[current_scene - 1].strips[cursor[Y]].midi_ch = std::stoi(temp_word);
								temp_word = 
									info->scenes[current_scene].strips[cursor[Y]].midi_ch == 0
									? "0" : std::to_string(info->scenes[current_scene].strips[cursor[Y]].midi_ch);
								transposition_field[cursor[Coordinates::Y]].set_cursor();
								break;
							case CursorX::VOLUME:
								info->scenes[current_scene - 1].strips[cursor[Y]].volume = std::stoi(temp_word);
								temp_word = 
									info->scenes[current_scene].strips[cursor[Y]].volume == 0
									? "0" : std::to_string(info->scenes[current_scene].strips[cursor[Y]].volume);
								volume_field[cursor[Coordinates::Y]].set_cursor();
								break;
							case CursorX::TRANSPOSITION:
								info->scenes[current_scene - 1].strips[cursor[Y]].transposition = std::stoi(temp_word);
								temp_word = 
									info->scenes[current_scene].strips[cursor[Y]].transposition == 0
									? "0" : std::to_string(info->scenes[current_scene].strips[cursor[Y]].transposition);
								transposition_field[cursor[Coordinates::Y]].set_cursor();
								break;
							case CursorX::L_SLIDER:
								double_X_slider[cursor[Coordinates::Y]].set_cursor_at_left();
								break;
							case CursorX::R_SLIDER:
								double_X_slider[cursor[Coordinates::Y]].set_cursor_at_right();
								break;
						}
					if ( keyboard_ptr->get_MIDI_state() == Switch::ON ) {
						// TO EDIT
						jack_midi_data_t to_edit_SysEx[] {0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x01, 0xF7};
						keyboard_ptr->send_page_SysEx(to_edit_SysEx);
						timer.sleep(2e8);

						keyboard_ptr->dump_scene( *info, current_scene );
					}
					will_dump = false;
				}
				break;/*}}}*/

			case KEY_LEFT :/*{{{*/
				if (cursor[Coordinates::X] > 0) {
					--cursor[Coordinates::X];
					if (cursor[Coordinates::Y] == -1) { // Zona de arriba
						info->scenes[current_scene].label = temp_word;
						temp_word = std::to_string(info->default_scene);
						vi_field.set_cursor();
					}
					else // Zona de objetos
						switch (cursor[Coordinates::X]) {
							case CursorX::STATE : // Status <- Instrumento
								info->scenes[current_scene].
									strips[cursor[Y]].
									midi_ch = std::stoi(temp_word);
								status_field[cursor[Coordinates::Y]].set_cursor();
								break;

							case CursorX::MIDI_CH:
								keyboard_ptr->set_instrument_name(
										info->program.bnk,
										info->program.num,
										cursor[Y], temp_word);
								temp_word =
									info->scenes[current_scene].strips[cursor[Y]].midi_ch == 0 ?
									"0" : std::to_string(
									info->scenes[current_scene].strips[cursor[Y]].midi_ch);
								channel_field[cursor[Coordinates::Y]].set_cursor();
								break;

							case CursorX::INSTRUMENT : // Instrumento <- Volumen
								info->scenes[current_scene].
									strips[cursor[Y]].volume =
									std::stoi(temp_word);
								temp_word = keyboard_ptr->get_instrument_name(
										info->program.bnk,
										info->program.num, cursor[Y]);
								instrument_field[cursor[Y]].set_cursor();
								break;
							case CursorX::VOLUME: // Volumen <- Transposition
								info->scenes[current_scene].strips[cursor[Y]].
									transposition = std::stoi(temp_word);
								temp_word =
									info->scenes[current_scene].strips[cursor[Y]].volume == 0 ?
									"0" : std::to_string(
									info->scenes[current_scene].strips[cursor[Y]].volume);
								volume_field[cursor[Coordinates::Y]].set_cursor();
								break;
							case CursorX::TRANSPOSITION : // Transposition <- Left slider
								double_X_slider[cursor[Coordinates::Y]].leave_cursor();
								temp_word = 
									info->scenes[current_scene].strips[cursor[Y]].transposition == 0
									? "0" : std::to_string(
									info->scenes[current_scene].strips[cursor[Y]].transposition);
								transposition_field[cursor[Coordinates::Y]].set_cursor();
								break;
							case CursorX::L_SLIDER :
								double_X_slider[ cursor[ Coordinates::Y ] ].swap_cursor();
								break;
						}
					will_dump = true;
				}
				break;/*}}}*/

			case KEY_RIGHT :/*{{{*/
				// Default scene
				if (cursor[Coordinates::Y] == -1) {
					if (cursor[Coordinates::X] == CursorX::DEFAULT_SCENE) {
						info->default_scene = std::stoi(temp_word);
						++cursor[Coordinates::X];
						temp_word = info->scenes[current_scene].label;
						etiqueta_field.set_cursor();
					}
				}
				else if (cursor[Coordinates::X] < CursorX::R_SLIDER) {
					++cursor[Coordinates::X];
					switch (cursor[Coordinates::X]) {
						case CursorX::MIDI_CH : // Volumen -> Transposition
							status_field[cursor[Coordinates::Y]].leave_cursor();
							temp_word = 
								info->scenes[current_scene].strips[cursor[Coordinates::Y]].midi_ch == 0
								? "0" : std::to_string(
								info->scenes[current_scene].strips[cursor[Coordinates::Y]].midi_ch);
							channel_field[cursor[Coordinates::Y]].set_cursor();
							break;
						case CursorX::INSTRUMENT : // check -> Instrument
							status_field[cursor[Coordinates::Y]].leave_cursor();
							temp_word = keyboard_ptr->get_instrument_name(
									info->program.bnk, info->program.num, cursor[Y] );
							instrument_field[cursor[Coordinates::Y]].set_cursor();
							break;
						case CursorX::VOLUME : // Instrument -> volumen
							keyboard_ptr->set_instrument_name(
									info->program.bnk, info->program.num, cursor[Y], temp_word );
							temp_word = 
								info->scenes[current_scene].strips[ cursor[Y] ].volume == 0
								? "0" : std::to_string(
								info->scenes[ current_scene ].strips[ cursor[Y] ].volume );
							volume_field[ cursor[ Coordinates::Y ] ].set_cursor();
							break;
						case CursorX::TRANSPOSITION : // Volumen -> Transposition
							info->scenes[ current_scene ].strips[ cursor[Y] ].volume =
								std::stoi( temp_word );
							temp_word = 
								info->scenes[current_scene].strips[ cursor[Y] ].transposition == 0
								? "0" : std::to_string(
								info->scenes[ current_scene ].strips[ cursor[Y] ].transposition );
							transposition_field[ cursor[ Coordinates::Y ] ].set_cursor();
							break;
						case CursorX::L_SLIDER : // Transposition -> Left slider
							info->scenes[ current_scene ].strips[ cursor[Y] ].transposition =
								std::stoi( temp_word );
							double_X_slider[ cursor[ Coordinates::Y ] ].set_cursor_at_left();
							break;
						case CursorX::R_SLIDER :
							double_X_slider[ cursor[ Coordinates::Y ] ].swap_cursor();
							break;
					}
					will_dump = true;
				}
				break;/*}}}*/

			case KEY_DOWN :/*{{{*/
				if (cursor[Coordinates::Y] < 15) { // Si puede bajar todavía
					++cursor[Coordinates::Y];
					switch (cursor[Coordinates::X]) {
						case CursorX::STATE: // Check
							if (cursor[Coordinates::Y] == 0) // viene de arriba
								info->default_scene = std::stoi(temp_word);
							else
								status_field[cursor[Coordinates::Y] - 1].leave_cursor();
							status_field[cursor[Coordinates::Y]].set_cursor();
							break;

						case CursorX::MIDI_CH : // Transposition
							if (cursor[ Coordinates::Y] > 0 ) // NO viene de arriba
								info->scenes[current_scene].strips[cursor[Y] - 1].midi_ch =
									std::stoi(temp_word);
							temp_word =
								info->scenes[current_scene].strips[cursor[Y]].midi_ch == 0 ?
								"0" : std::to_string(
								info->scenes[current_scene].strips[cursor[Y]].midi_ch);
							channel_field[cursor[Y]].set_cursor();
							break;

						case CursorX::INSTRUMENT: // Instrument
							if (cursor[Coordinates::Y] == 0) {
								info->scenes[current_scene].label = temp_word;
								temp_word = 
									info->scenes[current_scene].strips[cursor[Y]].volume == 0 ?
									"0" : std::to_string(
									info->scenes[current_scene].strips[cursor[Y]].volume);
									volume_field[cursor[Coordinates::Y]].set_cursor();
									cursor[Coordinates::X] = 2;
							}
							else {
								keyboard_ptr->set_instrument_name(
										info->program.bnk, info->program.num,
										cursor[Y] - 1, temp_word);
								temp_word = keyboard_ptr->get_instrument_name(
										info->program.bnk, info->program.num, cursor[Y] );
								instrument_field[cursor[ Coordinates::Y ]].set_cursor();
							}
							break;

						case CursorX::VOLUME: // Volumen
							if (cursor[Coordinates::Y] == 0) // Venía de arriba
								info->scenes[current_scene].label = temp_word;
							else
								info->scenes[ current_scene ].strips[ cursor[Y] - 1 ].volume =
									std::stoi( temp_word );
							temp_word = 
								info->scenes[current_scene].strips[cursor[Y]].volume == 0 ?
								"0" : std::to_string(
								info->scenes[current_scene].strips[cursor[Y]].volume);
								volume_field[cursor[Coordinates::Y]].set_cursor();
							break;

						case CursorX::TRANSPOSITION : // Transposition
							if ( cursor[ Coordinates::Y ] == 0 )
								info->scenes[ current_scene ].label = temp_word;
							else
								info->scenes[ current_scene ].strips[ cursor[Y] - 1 ].transposition =
									std::stoi( temp_word );
							temp_word = 
								info->scenes[current_scene].strips[ cursor[Y] ].transposition == 0 ?
								"0" : std::to_string(
								info->scenes[ current_scene ].strips[ cursor[Y] ].transposition );
							transposition_field[ cursor[Y] ].set_cursor();
							break;
						case CursorX::L_SLIDER : // Left Slider
							if ( cursor[ Coordinates::Y ] == 0 )
								info->scenes[ current_scene ].label = temp_word;
							else
								double_X_slider[ cursor[ Coordinates::Y ] - 1 ].leave_cursor();
							double_X_slider[ cursor[ Coordinates::Y ] ].set_cursor_at_left();
							break;
						case CursorX::R_SLIDER:
							// Análogo a case 4
							if ( cursor[ Coordinates::Y ] == 0 )
								info->scenes[ current_scene ].label = temp_word;
							else
								double_X_slider[ cursor[ Coordinates::Y ] - 1 ].leave_cursor();
							double_X_slider[ cursor[ Coordinates::Y ] ].set_cursor_at_right();
							break;
					}
					will_dump = true;
				}
				break;/*}}}*/

			case KEY_UP :/*{{{*/
				if (cursor[Coordinates::Y] > -1) { // aún no está en la label
					--cursor[Coordinates::Y];
					switch (cursor[ Coordinates::X]) {
						case CursorX::STATE: // check: nada que salvar
							status_field[cursor[Coordinates::Y] + 1].leave_cursor();
							if (cursor[Coordinates::Y ] == -1) {
								temp_word = std::to_string(info->default_scene);
								vi_field.set_cursor();
							}
							else
								status_field[ cursor[ Coordinates::Y ] ].set_cursor();
							break;

						case CursorX::MIDI_CH: // transposition
							info->scenes[current_scene].strips[cursor[Coordinates::Y] + 1].midi_ch =
								std::stoi(temp_word);
							if (cursor[Coordinates::Y] == -1) {
								temp_word = std::to_string(info->default_scene);
								vi_field.set_cursor();
								cursor[Coordinates::X] = CursorX::DEFAULT_SCENE;
							}
							else {
								temp_word = 
									info->scenes[current_scene].strips[cursor[Y]].midi_ch == 0
									? "0" : std::to_string(
									info->scenes[current_scene].strips[cursor[Y]].midi_ch);
								channel_field[cursor[Coordinates::Y]].set_cursor();
							}
							break;
						case CursorX::INSTRUMENT: // instrumento: se salva siempre
							keyboard_ptr->set_instrument_name(
									info->program.bnk, info->program.num, cursor[Y] + 1, temp_word );
							if (cursor[ Coordinates::Y] == -1 ) {
								temp_word = info->scenes[current_scene].label;
								etiqueta_field.set_cursor();
							}
							else {
								temp_word = keyboard_ptr->get_instrument_name(
										info->program.bnk, info->program.num, cursor[Y] );
								instrument_field[ cursor[ Coordinates::Y ] ].set_cursor();
							}
							break;
						case CursorX::VOLUME :
							info->scenes[current_scene].strips[cursor[Y] + 1].volume =
								std::stoi( temp_word );
							if (cursor[ Coordinates::Y ] == -1) {
								temp_word = info->scenes[current_scene].label;
								cursor[Coordinates::X] = 1;
								etiqueta_field.set_cursor();
							}
							else {
								temp_word = 
									info->scenes[current_scene].strips[ cursor[Y] ].volume == 0
									? "0" : std::to_string(
									info->scenes[ current_scene ].strips[ cursor[Y] ].volume );
								volume_field[cursor[ Coordinates::Y]].set_cursor();
							}
							break;
						case CursorX::TRANSPOSITION: // transposition
							info->scenes[ current_scene ].strips[ cursor[Y] + 1 ].transposition =
								std::stoi( temp_word );
							if ( cursor[ Coordinates::Y ] == -1 ) {
								temp_word = info->scenes[ current_scene ].label;
								etiqueta_field.set_cursor();
							}
							else {
								temp_word = 
									info->scenes[current_scene].strips[ cursor[Y] ].transposition == 0
									? "0" : std::to_string(
									info->scenes[ current_scene ].strips[ cursor[Y] ].transposition );
								transposition_field[ cursor[ Coordinates::Y ] ].set_cursor();
							}
							break;
						case CursorX::L_SLIDER:
							double_X_slider[ cursor[ Coordinates::Y ] + 1 ].leave_cursor();
							if ( cursor[ Coordinates::Y ] == -1 ) {
								temp_word = info->scenes[ current_scene ].label;
								etiqueta_field.set_cursor();
							}
							else
								double_X_slider[ cursor[Coordinates::Y] ].set_cursor_at_left();
							break;
						case CursorX::R_SLIDER:
							double_X_slider[ cursor[ Coordinates::Y] + 1 ].leave_cursor();
							if (cursor[Coordinates::Y] == -1) {
								temp_word = info->scenes[ current_scene ].label;
								etiqueta_field.set_cursor();
							}
							else
								double_X_slider[ cursor[Coordinates::Y] ].set_cursor_at_right();
							break;
					}
					will_dump = true;
				}
				break;/*}}}*/

			case ' ' :/*{{{*/
				if (cursor[Y] == -1) { // Hasta arriba
					if (cursor[X] == 1) {
						temp_word.append(" ");
						etiqueta_field.set_content(temp_word);
					}
				}
				else if (cursor[X] == CursorX::STATE) {
					if (info->scenes[current_scene].strips[cursor[Y]].
							state == Switch::OFF) {
						info->scenes[current_scene].strips[cursor[Y]].
							state = Switch::ON;
						status_field[cursor[Y]].on();
						instrument_field[cursor[Y]].on();
						volume_field[cursor[Y]].on();
						transposition_field[cursor[Y]].on();
						double_X_slider[cursor[Y]].on();
					}
					else {
						info->scenes[current_scene].strips[cursor[Y]].
							state = Switch::OFF;
						status_field[cursor[Y]].off();
						instrument_field[cursor[Y]].off();
						volume_field[cursor[Y]].off();
						transposition_field[cursor[Y]].off();
						double_X_slider[cursor[Y]].off();
					}

					status_field[cursor[Y]].set_cursor();
					instrument_field[cursor[Y]].set_text(
							keyboard_ptr->get_instrument_name(
								info->program.bnk, info->program.num, 
								cursor[Y]));
					volume_field[cursor[Y]].set_value(
							info->scenes[current_scene].strips[cursor[Y]].
							volume);
					transposition_field[cursor[Y]].set_value(
							info->scenes[current_scene].strips[cursor[Y]].
							transposition);
				}
				else if (cursor[X] == CursorX::INSTRUMENT) {
					temp_word.append(" ");
					instrument_field[cursor[Y]].set_text(temp_word);
				}
				will_dump = true;

				break;/*}}}*/

			case KEY_BACKSPACE : case 8:/*{{{*/
				if (cursor[Coordinates::Y] == -1) { // Hasta arriba
					temp_word = temp_word.substr(0, temp_word.length() - 1);
					cursor[Coordinates::X] == 0
						? vi_field.set_content(temp_word)
						: etiqueta_field.set_content(temp_word);
				}
				else if (cursor[Coordinates::X] == CursorX::STATE) {
					temp_word = temp_word.substr(0, temp_word.length() - 1);
					transposition_field[ cursor[Y] ].set_text( temp_word );
				}
				else if (cursor[Coordinates::X] == CursorX::MIDI_CH) {
					temp_word = temp_word.substr(0, temp_word.length() - 1);
					channel_field[cursor[Y]].set_text(temp_word);
				}
				else if (cursor[Coordinates::X] == CursorX::INSTRUMENT) {
					temp_word = temp_word.substr(0, temp_word.length() - 1);
					instrument_field[cursor[Y]].set_text(temp_word);
				}
				else if ( cursor[Coordinates::X] == CursorX::VOLUME) {
					temp_word = temp_word.substr(0, temp_word.length() - 1);
					volume_field[cursor[Y]].set_text(temp_word);
				}
				else if ( cursor[Coordinates::X] == CursorX::TRANSPOSITION ) {
					temp_word = temp_word.substr( 0, temp_word.length() - 1 );
					transposition_field[ cursor[Y] ].set_text( temp_word );
				}
				will_dump = true;

				break;/*}}}*/

			case '<' : // {{{
				if (cursor[Coordinates::Y] > -1) { // Zona de controles
					if (cursor[X] == CursorX::L_SLIDER) {
						if (double_X_slider[cursor[Y]].
								decrease_left_slider() == Moved::YES)
							--(info->scenes[current_scene].
									strips[cursor[Y]].lower_key);
					}
					else if (cursor[X] == CursorX::R_SLIDER) {
						if (double_X_slider[cursor[Y]].
								decrease_right_slider() == Moved::YES)
							--(info->scenes[current_scene].
									strips[ cursor[Y] ].upper_key);
					}
					will_dump = true;
				}

				break;/*}}}*/

			case '>' : // {{{

				if (cursor[Coordinates::Y] > -1) {
					if (cursor[Coordinates::X] == CursorX::L_SLIDER) {
						if (double_X_slider[cursor[Y]].
								increase_left_slider() == Moved::YES)
							++(info->scenes[current_scene].
									strips[cursor[Y]].lower_key);
					}
					else if (cursor[Coordinates::X] == CursorX::R_SLIDER){
						if (double_X_slider[cursor[Y]].
								increase_right_slider() == Moved::YES)
							++(info->scenes[current_scene].
									strips[cursor[Y]].upper_key);
					}
					will_dump = true;
				}
				break;/*}}}*/

			case '\n' :/*{{{*/
				if (cursor[Coordinates::Y] == -1)
					if (cursor[Coordinates::X] == CursorX::DEFAULT_SCENE)
						info->default_scene = std::stoi(temp_word);
					else
						info->scenes[current_scene].label = temp_word;
				else switch (cursor[Coordinates::X]) {
					case CursorX::MIDI_CH :
						info->scenes[current_scene].strips[cursor[Y]].midi_ch =
							temp_word == "" ? 0 : std::stoi(temp_word);
						break;
					case CursorX::INSTRUMENT :
						keyboard_ptr->set_instrument_name(
								info->program.bnk, info->program.num, cursor[Y], temp_word );
						break;
					case CursorX::VOLUME :
						info->scenes[ current_scene ].strips[ cursor[Y] ].volume =
							temp_word == "" ? 0 : std::stoi( temp_word );
						break;
					case CursorX::TRANSPOSITION :
						info->scenes[current_scene].strips[cursor[Y]].transposition =
							temp_word == "" ? 0 : std::stoi(temp_word);
						break;
					default:
						break;
				 }

				// Dumpeamos sí o sí
				if ( keyboard_ptr->get_MIDI_state() == Switch::ON ) {
						jack_midi_data_t to_edit_SysEx[] {0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x01, 0xF7};
						keyboard_ptr->send_page_SysEx(to_edit_SysEx);
						timer.sleep(2e8);

					keyboard_ptr->dump_scene( *info, current_scene );
				}

				break;/*}}}*/

			case 27 : // ESCAPE{{{
				curs_set( false );
				hide();
				update_popups();
				again = false;

				will_dump = false;

				break;/*}}}*/

			case 1 : // DUplicate{{{
				if (info->n_scenes < MAX_VARIATIONS) {
					info->scenes.push_back(Scene {});
					for (int32_t i = info->n_scenes - 1; i >= current_scene; --i )
						info->scenes[ i + 1 ] = info->scenes[ i ];
					++info->n_scenes;
					update();

					if (cursor[Coordinates::Y] == -1 ) { // si está hasta arriba
						if (cursor[X] == CursorX::DEFAULT_SCENE) {
							info->default_scene = std::stoi(temp_word);
							vi_field.set_content( std::to_string(info->default_scene));
							vi_field.set_cursor();
						}
						else {
							info->scenes[current_scene - 1].label = temp_word; // guardamos
							temp_word = info->scenes[ current_scene ].label; // copiamos nuevo
							etiqueta_field.set_cursor();
						}
					}
					else
						switch (cursor[Coordinates::X]) {
							case CursorX::STATE:
								status_field[ cursor[Y] ].set_cursor();
								break;
							case CursorX::MIDI_CH:
								info->scenes[current_scene + 1].strips[cursor[Coordinates::Y]].midi_ch =
									std::stoi(temp_word);
								temp_word =
									info->scenes[current_scene].strips[cursor[Coordinates::Y]].midi_ch == 0
									? "0" : std::to_string(
									info->scenes[current_scene].strips[cursor[Coordinates::Y]].midi_ch);
								channel_field[cursor[Coordinates::Y]].set_cursor();
								break;
							case CursorX::INSTRUMENT:
								keyboard_ptr->set_instrument_name(
										info->program.bnk, info->program.num, cursor[Y], temp_word );
								instrument_field[ cursor[Y] ].set_cursor();
								break;
							case CursorX::VOLUME:
								info->scenes[ current_scene + 1 ].strips[ cursor[Y] ].volume =
									std::stoi( temp_word );
								temp_word = 
									info->scenes[current_scene].strips[ cursor[Y] ].volume == 0
									? "0" : std::to_string(
									info->scenes[ current_scene ].strips[ cursor[Y] ].volume );
								volume_field[ cursor[Y] ].set_cursor();
								break;
							case CursorX::TRANSPOSITION:
								info->scenes[current_scene + 1].strips[cursor[Coordinates::Y]].transposition =
									std::stoi(temp_word);
								temp_word =
									info->scenes[current_scene].strips[cursor[Coordinates::Y]].transposition == 0
									? "0" : std::to_string(
									info->scenes[current_scene].strips[cursor[Coordinates::Y]].transposition);
								transposition_field[cursor[Coordinates::Y]].set_cursor();
								break;
							case CursorX::L_SLIDER:
								double_X_slider[ cursor[Coordinates::Y] ].set_cursor_at_left();
								break;
							case CursorX::R_SLIDER:
								double_X_slider[ cursor[Coordinates::Y ] ].set_cursor_at_right();
								break;
						}
					if ( keyboard_ptr->get_MIDI_state() == Switch::ON ) {
						jack_midi_data_t to_edit_SysEx[] {0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x01, 0xF7};
						keyboard_ptr->send_page_SysEx(to_edit_SysEx);
						timer.sleep(2e8);

						keyboard_ptr->dump_scene( *info, current_scene );
					}
					will_dump = false;
					++current_scene;
				}

				break;/*}}}*/

			case 18 : // Eliminate{{{
				if (info->n_scenes > 1) {
					if ( current_scene == info->n_scenes - 1 )
						--current_scene;
					for ( int32_t i = current_scene; i < info->n_scenes - 1; ++i )
						info->scenes[i] = info->scenes[i + 1];
					--info->n_scenes;
					update();

					if (cursor[Coordinates::Y] == -1) { // si está hasta arriba
						if (cursor[Coordinates::X] == CursorX::DEFAULT_SCENE) {
							info->default_scene = std::stoi(temp_word);
							vi_field.set_content( std::to_string(info->default_scene));
							vi_field.set_cursor();
						}
						else {
							info->scenes[ current_scene - 1 ].label = temp_word; // guardamos
							temp_word = info->scenes[ current_scene ].label; // copiamos nuevo
							etiqueta_field.set_cursor();
						}
					}
					else
						switch (cursor[Coordinates::X]) {
							case CursorX::STATE:
								status_field[cursor[Y]].set_cursor();
								break;
							case CursorX::MIDI_CH:
								info->scenes[current_scene + 1].strips[cursor[Coordinates::Y]].midi_ch =
									std::stoi(temp_word);
								temp_word =
									info->scenes[current_scene].strips[cursor[Coordinates::Y]].midi_ch == 0
									? "0" : std::to_string(
									info->scenes[current_scene].strips[cursor[Coordinates::Y]].midi_ch);
								channel_field[cursor[Coordinates::Y]].set_cursor();
								break;
							case CursorX::INSTRUMENT:
								keyboard_ptr->set_instrument_name(
										info->program.bnk, info->program.num, cursor[Y], temp_word );
								instrument_field[ cursor[Y] ].set_cursor();
								break;
							case CursorX::VOLUME:
								info->scenes[ current_scene + 1 ].strips[ cursor[Y] ].volume =
									std::stoi( temp_word );
								temp_word = 
									info->scenes[current_scene].strips[ cursor[Y] ].volume == 0
									? "0" : std::to_string(
									info->scenes[ current_scene ].strips[ cursor[Y] ].volume );
								volume_field[ cursor[Y] ].set_cursor();
								break;
							case CursorX::TRANSPOSITION:
								info->scenes[ current_scene + 1 ].strips[ cursor[Y] ].transposition =
									std::stoi( temp_word );
								temp_word = 
									info->scenes[current_scene].strips[ cursor[Y] ].transposition == 0
									? "0" : std::to_string(
									info->scenes[ current_scene ].strips[ cursor[Y] ].transposition );
								transposition_field[ cursor[Y] ].set_cursor();
								break;
							case CursorX::L_SLIDER:
								double_X_slider[ cursor[Coordinates::Y] ].set_cursor_at_left();
								break;
							case CursorX::R_SLIDER:
								double_X_slider[ cursor[Coordinates::Y ] ].set_cursor_at_right();
								break;
						}
					/*
					if ( keyboard_ptr->get_MIDI_state() == Switch::ON ) {
						jack_midi_data_t to_edit_SysEx[] {0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x01, 0xF7};
						keyboard_ptr->send_page_SysEx(to_edit_SysEx);
						timer.sleep(2e8);

						keyboard_ptr->dump_scene( *info, current_scene );
					}
					*/
					will_dump = false;
					info->scenes.pop_back();
				}
				break;/*}}}*/

			case 3: // <C-c> COPY_ONE_VARIATION{{{
				clipboard.n_variaciones = 1;
				clipboard.variacion[0] = info->scenes[ current_scene ];
				break;/*}}}*/

			case 25: // <C-y> YANK_ALL_VARIATIONS{{{
				clipboard.n_variaciones = info->n_scenes;
				for ( int32_t i = 0; i < info->n_scenes; ++i )
					clipboard.variacion[i] = info->scenes[ i ];
				break;/*}}}*/

			case 16: // <C-p> PASTE_VARIATIONS{{{
				// comprobamos que quepan
				if ( MAX_VARIATIONS - info->n_scenes >= clipboard.n_variaciones ) {
					// empujamos
					for ( int16_t i = current_scene; i < info->n_scenes; ++i )
						info->scenes[ i + clipboard.n_variaciones ] = clipboard.variacion[i];
					// metemos el clipboard
					for ( int16_t i = 0; i < clipboard.n_variaciones; ++i )
						info->scenes[current_scene + i] = clipboard.variacion[i];
					// Incrementamos n_scenes
					info->n_scenes += clipboard.n_variaciones;

					update();

					if (cursor[Coordinates::Y] == -1 ) { // si está hasta arriba
						if (cursor[X] == CursorX::DEFAULT_SCENE) {
							info->default_scene = std::stoi(temp_word);
							vi_field.set_content( std::to_string(info->default_scene));
							vi_field.set_cursor();
						}
						else {
							info->scenes[current_scene - 1].label = temp_word; // guardamos
							temp_word = info->scenes[ current_scene ].label; // copiamos nuevo
							etiqueta_field.set_cursor();
						}
					}

					else switch (cursor[Coordinates::X]) {
						case CursorX::STATE:
							status_field[cursor[Y]].set_cursor();
							break;
						case CursorX::MIDI_CH:
							info->scenes[current_scene + 1].strips[cursor[Y]].midi_ch =
								std::stoi(temp_word);
							temp_word =
								info->scenes[current_scene].strips[cursor[Y]].midi_ch == 0
								? "0" : std::to_string(
								info->scenes[current_scene].strips[cursor[Y]].midi_ch);
							channel_field[cursor[Y]].set_cursor();
							break;
						case CursorX::INSTRUMENT:
							keyboard_ptr->set_instrument_name(
									info->program.bnk, info->program.num, cursor[Y], temp_word );
							instrument_field[ cursor[Y] ].set_cursor();
							break;
						case CursorX::VOLUME:
							info->scenes[ current_scene + 1 ].strips[ cursor[Y] ].volume =
								std::stoi( temp_word );
							temp_word = 
								info->scenes[current_scene].strips[ cursor[Y] ].volume == 0
								? "0" : std::to_string(
								info->scenes[ current_scene ].strips[ cursor[Y] ].volume );
							volume_field[ cursor[Y] ].set_cursor();
							break;
						case CursorX::TRANSPOSITION:
							info->scenes[current_scene + 1].strips[cursor[Y]].transposition =
								std::stoi(temp_word);
							temp_word =
								info->scenes[current_scene].strips[cursor[Y]].transposition == 0
								? "0" : std::to_string(
								info->scenes[current_scene].strips[cursor[Y]].transposition);
							transposition_field[ cursor[Y] ].set_cursor();
							break;
						case CursorX::L_SLIDER:
							double_X_slider[ cursor[Coordinates::Y] ].set_cursor_at_left();
							break;
						case CursorX::R_SLIDER:
							double_X_slider[ cursor[Coordinates::Y ] ].set_cursor_at_right();
							break;
					}

					if ( keyboard_ptr->get_MIDI_state() == Switch::ON ) {
						jack_midi_data_t to_edit_SysEx[] {0xF0, 0x42, 0x30, 0x7A, 0x4E, 0x01, 0xF7};
						keyboard_ptr->send_page_SysEx(to_edit_SysEx);
						timer.sleep(2e8);

						keyboard_ptr->dump_scene( *info, current_scene );
					}
					will_dump = false;
				}
				break;/*}}}*/

			default :/*{{{*/
				if (tecla == 32 or (44 <= tecla and tecla <= 122) or tecla == 263) {

					if (cursor[Coordinates::Y] == -1) {
						sprintf(tecla_c_string, "%c", tecla);
						temp_word = temp_word + tecla_c_string;
						cursor[Coordinates::X] == CursorX::DEFAULT_SCENE
							? vi_field.set_content(temp_word)
							: etiqueta_field.set_content(temp_word);
					}
					else switch (cursor[Coordinates::X]) {
						case CursorX::MIDI_CH :
							sprintf(tecla_c_string, "%c", tecla);
							temp_word = temp_word + tecla_c_string;
							channel_field[cursor[Coordinates::Y]].set_text(temp_word);
							break;
						case CursorX::INSTRUMENT:
							sprintf( tecla_c_string, "%c", tecla );
							temp_word = temp_word + tecla_c_string;
							instrument_field[ cursor[Y] ].set_text( temp_word );
							break;
						case CursorX::VOLUME :
							sprintf( tecla_c_string, "%c", tecla );
							temp_word = temp_word + tecla_c_string;
							volume_field[ cursor[Y] ].set_text( temp_word );
							break;
						case CursorX::TRANSPOSITION :
							sprintf(tecla_c_string, "%c", tecla);
							temp_word = temp_word + tecla_c_string;
							transposition_field[cursor[Coordinates::Y]].set_text(temp_word);
							break;
						default:
							break;
					}
					will_dump = false;
				}
				break;/*}}}*/
		}
		/* Por ahora eliminamos esta parte, vamos a reservar el dumping a la tecla ENTER
		 * y a los cambios de variación. Y probaremos cómo se siente!!
		 */
		if ( will_dump and keyboard_ptr->get_MIDI_state() == Switch::ON ) {
			// keyboard_ptr->select_page( Page::TIMBRE );
			// keyboard_ptr->dump_scene( *info, scenes );
		}

	} while (again);
}/*}}}*/

void Orchestra::reset_variation() noexcept/*{{{*/
{
	current_scene = 0;
}/*}}}*/

auto Orchestra::add_empty_scene(Performance*& _Performance) noexcept -> void {/*{{{*/
	info = _Performance;
	++info->n_scenes;
	info->scenes.push_back(Scene {});
}/*}}}*/
