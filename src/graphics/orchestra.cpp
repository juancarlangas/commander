#include "graphics/orchestra.hpp"
#include "elements/windows/popups/text_popup.hpp"
#include "elements/windows/popups/double_X_slider.hpp"
#include "graphics/colors.hpp"
#include "graphics/ncurses.hpp"
#include "printing.hpp"
#include "windows.hpp"
#include <ncurses.h>
#include <panel.h>
#include <string>
#include <iostream>

using Coordinates::X;
using Coordinates::Y;

Orchestra::Orchestra() :
	native_font { { { GREEN_DEFAULT, "Bold" },/*{{{*/
					{ BLUE_DEFAULT, "Bold" },
					{ YELLOW_DEFAULT, "Bold" },
					{ BLUE_DEFAULT, "Regular" },
					{ MAGENTA_DEFAULT, "Regular" },
					{ YELLOW_DEFAULT, "Regular" },
					{ CYAN_DEFAULT, "Bold" },
					{ RED_DEFAULT, "Bold " } } },
	cursor_font { WHITE_DEFAULT, "Bold" },
	MIDI_font { GREEN_DEFAULT, "Bold" },
	dimmed_font { GRAY_DEFAULT, "Bold" }
{}/*}}}*/

void Orchestra::link_MIDI_device( Keyboard *_Teclado ) noexcept
{/*{{{*/
	keyboard = _Teclado;
}/*}}}*/

void Orchestra::init( const int32_t _Ysize, const int32_t _Xsize,
						const int32_t _Ypos, const int32_t _Xpos ) noexcept
{/*{{{*/
	variacion = 0;

	// Base
	base.init( _Ysize, _Xsize, _Ypos, _Xpos, 1 );
	base.set_box_color( WHITE_DEFAULT );
	base.set_box_width( "Bold" );
	base.define_box( 0, 0, 0, 0, 0, 0, 0, 0 );
	base.update();

	// Indicador de variación
	variacion_text_box.init( 3, 20, _Ypos + 1, _Xpos + 2 );
	variacion_text_box.set_font_color( GRAY_DEFAULT );
	variacion_text_box.set_font_width( "Bold" );
	variacion_text_box.update();

	// keyboard_text_box
	keyboard_scheme.init( 5, 61, _Ypos + ( _Ysize * 40 / 200 ), _Xpos + ( _Xsize * 80 / 200 ) );
	keyboard_scheme.set_font_color( WHITE_DEFAULT );
	keyboard_scheme.set_font_width( "Bold" );
	keyboard_scheme.auto_draw();
	keyboard_scheme.update();

	// Etiqueta
	etiqueta_field.create( "Etiqueta", 3, _Xsize * 80 / 200, _Ypos + 1, _Xpos + ( _Xsize / 4 ),
							STRING, 16 );
	etiqueta_field.set_font_width( "Bold" );
	etiqueta_field.set_font_color( WHITE_DEFAULT );

	int32_t y_starting_point = _Ypos + ( _Ysize * 70 / 200 );

	for ( int32_t i = 0; i < 8; ++i ) {
		status_field[i].init( y_starting_point + i, _Xpos + 2,
				native_font[ i ], cursor_font, dimmed_font );
		status_field[i].update();

		instrument_field[i].init( 1, 32, y_starting_point + i, _Xpos + 5,
				native_font[ i ], cursor_font, dimmed_font );
		instrument_field[i].update();

		transposition_field[i].init( 1, 4, y_starting_point + i, _Xpos + 39,
				native_font[ i ], cursor_font, dimmed_font );
		transposition_field[i].update();
		
		// invocamos su clase base porque no cuadra con su init
		double_X_slider[i].Popup::init( 1, 62, y_starting_point + i, _Xpos + 44,
				native_font[ i ], cursor_font, dimmed_font );
		double_X_slider[i].update();
	}

	hide();
}/*}}}*/

void KeyboardScheme::auto_draw() noexcept
{/*{{{*/
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

void Orchestra::show( struct System *&_Row ) noexcept
{/*{{{*/
	// solo obtiene la información y aparece los páneles

	info = _Row;

	base.show();
	variacion_text_box.show();
	keyboard_scheme.show();
	etiqueta_field.show();

	for ( int32_t i = 0; i < 8; ++i ) {
		status_field[i].show();
		instrument_field[i].show();
		transposition_field[i].show();
		double_X_slider[i].show();
	}

	update();
}/*}}}*/

void Orchestra::update() noexcept
{/*{{{*/
	variacion_text_box.set_text( "Variacion " + std::to_string( variacion + 1 ) +
								" de " + std::to_string( info->n_variaciones ) );
	etiqueta_field.set_content( info->variacion[ variacion ].etiqueta );

	for ( int32_t i = 0; i < 8; ++i ) {

		// Preparando las dobles barras
		int16_t &l_Value = info->variacion[ variacion ].track[ i ].lower_key;
		int16_t &r_Value = info->variacion[ variacion ].track[ i ].upper_key;
		// Límites
		if ( r_Value < MIN_KEY )
			r_Value = MIN_KEY;
		if ( l_Value < MIN_KEY )
			l_Value = MIN_KEY;
		if ( l_Value > MAX_KEY )
			l_Value = MAX_KEY;
		if ( r_Value > MAX_KEY )
			r_Value = MAX_KEY;
		// cruzamientos
		if ( l_Value > r_Value )
			r_Value = l_Value;
		double_X_slider[i].set_values( l_Value, r_Value );

		if ( info->variacion[ variacion ].track[ i ].status == Switch::ON ) {
			status_field[ i ].on();
			instrument_field[ i ].on();
			transposition_field[ i ].on();
			double_X_slider[ i ].on();
		}
		else {
			status_field[ i ].off();
			instrument_field[ i ].off();
			transposition_field[ i ].off();
			double_X_slider[ i ].off();
		}

		instrument_field[ i ].set_text( info->instrumento[ i ] );
		transposition_field[ i ].set_text(
				std::to_string( info->variacion[ variacion ].track[ i ].transposition ) );
	}
}/*}}}*/

void Orchestra::hide() noexcept
{/*{{{*/
	etiqueta_field.hide();
	variacion_text_box.hide();
	keyboard_scheme.hide();
	base.hide();
}/*}}}*/

void Orchestra::capture_key() noexcept
{/*{{{*/
	// este arreglo cursor[2] (coordenadas X, Y) representa la malla por donde transitará
	// virtualmente el cursor. 5 columnas por 9 filas. El Y=-1 representa la posición de la
	// etiqueta. Cuando cursor[Y] == -1, dicha etiqueta obtendrá el cursor independiéntemente
	// del valor de cursor[X]
	cursor[ X ] = 0;
	cursor[ Y ] = -1; // la etiqueta estará en el -1 para concordancia con los otros objetos

	curs_set( true );
	etiqueta_field.set_cursor(); // Por defalt el cursor estará sobre la etiqueta

	/* Cadena para almacenar la palabra editada. Al momento de cambiar de text_popup
	 * temp_word será copiada diréctamente a la base de datos. Esto no aplica para el
	 * status_field o para el double_X_slider, ya que el efecto en la base de datos
	 * y sobre el teclado serán inmediatos */
	temp_word = info->variacion[ variacion ].etiqueta;
	int32_t tecla;
	char tecla_c_string[3];
	bool again = true;
	bool will_dump;

	do {
		switch ( tecla = getch() ) {
			case 11: // TOGGLE_MIDI_STATE{{{
				keyboard->toggle_MIDI_state();
				print_MIDI_state( MIDI_state_window, keyboard->get_MIDI_state() );

				if ( cursor[Y] == -1 ) // si está hasta arriba
					etiqueta_field.set_cursor();
				else
					switch ( cursor[X] ) {
						case 0:
							status_field[ cursor[Y] ].set_cursor();
							break;
						case 1:
							instrument_field[ cursor[Y] ].set_cursor();
							break;
						case 2:
							transposition_field[ cursor[Y] ].set_cursor();
							break;
						case 3:
							double_X_slider[ cursor[Coordinates::Y] ].set_cursor_at_left();
							break;
						case 4:
							double_X_slider[ cursor[Coordinates::Y ] ].set_cursor_at_right();
							break;
					}
				will_dump = false;

				break;/*}}}*/

			case 353 : // Variación anterior{{{
				if ( variacion > 0 ) { // si se puede subir
					--variacion;
					update();

					if ( cursor[Y] == -1 ) { // si está hasta arriba
						info->variacion[ variacion + 1 ].etiqueta = temp_word; // guardamos
						temp_word = info->variacion[ variacion ].etiqueta; // copiamos nuevo
						etiqueta_field.set_cursor();
					}
					else
						switch ( cursor[X] ) {
							case 0:
								status_field[ cursor[Y] ].set_cursor();
								break;
							case 1:
								info->instrumento[ cursor[Y] ] = temp_word;
								instrument_field[ cursor[Y] ].set_cursor();
								break;
							case 2:
								info->variacion[ variacion + 1 ].track[ cursor[Y] ].transposition =
									std::stoi( temp_word );
								temp_word = 
									info->variacion[variacion].track[ cursor[Y] ].transposition == 0
									? "0" : std::to_string(
									info->variacion[ variacion ].track[ cursor[Y] ].transposition );
								transposition_field[ cursor[Y] ].set_cursor();
								break;
							case 3:
								double_X_slider[ cursor[Coordinates::Y] ].set_cursor_at_left();
								break;
							case 4:
								double_X_slider[ cursor[Coordinates::Y ] ].set_cursor_at_right();
								break;
						}

					if ( keyboard->get_MIDI_state() == Switch::ON ) {
						keyboard->select_page( Page::TIMBRE );
						keyboard->dump_variation( *info, variacion );
					}
					will_dump = false;
				}
				break;/*}}}*/

			case 9 : // TAB{{{
				if ( variacion < info->n_variaciones - 1 ) {
					++variacion;
					update();
					if ( cursor[ Coordinates::Y ] == -1 ) {
						info->variacion[ variacion - 1 ].etiqueta = temp_word; // guardamos
						temp_word = info->variacion[ variacion ].etiqueta;
						etiqueta_field.set_cursor();
					}
					else
						switch ( cursor[ Coordinates::X ] ) {
							case 0:
								status_field[ cursor[ Coordinates::Y ] ].set_cursor();
								break;
							case 1:
								info->instrumento[ cursor[Y] ] = temp_word;
								temp_word = info->instrumento[ cursor[Y] ];
								instrument_field[ cursor[ Coordinates::Y ] ].set_cursor();
								break;
							case 2:
								info->variacion[ variacion - 1 ].track[ cursor[Y] ].transposition =
									std::stoi( temp_word );
								temp_word = 
									info->variacion[variacion].track[ cursor[Y] ].transposition == 0
									? "0" : std::to_string(
									info->variacion[ variacion ].track[ cursor[Y] ].transposition );
								transposition_field[ cursor[ Coordinates::Y ] ].set_cursor();
								break;
							case 3:
								double_X_slider[ cursor[Coordinates::Y ]].set_cursor_at_left();
								break;
							case 4:
								double_X_slider[ cursor[Coordinates::Y ]].set_cursor_at_right();
								break;
						}
					if ( keyboard->get_MIDI_state() == Switch::ON ) {
						keyboard->select_page( Page::TIMBRE );
						keyboard->dump_variation( *info, variacion );
					}
					will_dump = false;
				}
				break;/*}}}*/

			case KEY_LEFT :/*{{{*/
				if ( cursor[ Coordinates::X ] > 0 ) {
					--cursor[ Coordinates::X ];
					if ( cursor[ Coordinates::Y ] >= 0 ) // Zona de objetos
						switch ( cursor[ Coordinates::X ] ) {
							case 0 : // Check <- Instrumento
								info->instrumento[ cursor[Y] ] = temp_word;
								status_field[ cursor[ Coordinates::Y ] ].set_cursor();
								break;
							case 1 : // Instrumento <- Transposition
								info->variacion[ variacion ].track[ cursor[Y] ].transposition =
									std::stoi( temp_word );
								temp_word = info->instrumento[ cursor[Y] ];
								instrument_field[ cursor[Y] ].set_cursor();
								break;
							case 2 : // Transpotision <- Left slider
								double_X_slider[ cursor[ Coordinates::Y ] ].leave_cursor();
								temp_word = 
									info->variacion[variacion].track[ cursor[Y] ].transposition == 0
									? "0" : std::to_string(
									info->variacion[ variacion ].track[ cursor[Y] ].transposition );
								transposition_field[ cursor[ Coordinates::Y ] ].set_cursor();
								break;
							case 3 :
								double_X_slider[ cursor[ Coordinates::Y ] ].swap_cursor();
								break;
						}
					will_dump = true;
				}
				break;/*}}}*/

			case KEY_RIGHT :/*{{{*/
				if ( cursor[ Coordinates::X ] < 4 ) {
					++cursor[ Coordinates::X ];
					if ( cursor[ Coordinates::Y ] >= 0 )
						switch ( cursor[ Coordinates::X ] ) {
							case 1 : // check -> Instrument
								status_field[ cursor[ Coordinates::Y ] ].leave_cursor();
								temp_word = info->instrumento[ cursor [Y] ];
								instrument_field[ cursor[ Coordinates::Y ] ].set_cursor();
								break;
							case 2 : // Instrument -> transposition
								info->instrumento[ cursor[Y] ] = temp_word;
								temp_word = 
									info->variacion[variacion].track[ cursor[Y] ].transposition == 0
									? "0" : std::to_string(
									info->variacion[ variacion ].track[ cursor[Y] ].transposition );
								transposition_field[ cursor[ Coordinates::Y ] ].set_cursor();
								break;
							case 3 : // Transposition -> Left slider
								info->variacion[ variacion ].track[ cursor[Y] ].transposition =
									std::stoi( temp_word );
								double_X_slider[ cursor[ Coordinates::Y ] ].set_cursor_at_left();
								break;
							case 4 :
								double_X_slider[ cursor[ Coordinates::Y ] ].swap_cursor();
								break;
						}
					will_dump = true;
				}
				break;/*}}}*/

			case KEY_DOWN :/*{{{*/
				if ( cursor[ Coordinates::Y ] < 7 ) { // Si puede bajar todavía
					++cursor[ Coordinates::Y ];
					switch ( cursor[ Coordinates::X ] ) {
						case 0: // Check
							if ( cursor[ Coordinates::Y ] == 0 )
								info->variacion[ variacion ].etiqueta = temp_word;
							else
								status_field[ cursor[ Coordinates::Y ] - 1 ].leave_cursor();
							status_field[ cursor[ Coordinates::Y ] ].set_cursor();
							break;
						case 1:
							if ( cursor[ Coordinates::Y ] == 0 )
								info->variacion[ variacion ].etiqueta = temp_word;
							else
								info->instrumento[ cursor[Y] - 1 ] = temp_word;
							temp_word = info->instrumento[ cursor[Y] ];
							instrument_field[ cursor[ Coordinates::Y ] ].set_cursor();
							break;
						case 2:
							if ( cursor[ Coordinates::Y ] == 0 )
								info->variacion[ variacion ].etiqueta = temp_word;
							else
								info->variacion[ variacion ].track[ cursor[Y] - 1 ].transposition =
									std::stoi( temp_word );
							temp_word = 
								info->variacion[variacion].track[ cursor[Y] ].transposition == 0
								? "0" : std::to_string(
								info->variacion[ variacion ].track[ cursor[Y] ].transposition );
							transposition_field[ cursor[ Coordinates::Y ] ].set_cursor();
							break;
						case 3:
							// Si el cursor eje Y es 0, estará debajo de la etiqueta y no existirá
							// cursor[ Coordinates::Y ] - 1 por lo que habrá violación de acceso
							if ( cursor[ Coordinates::Y ] == 0 )
								info->variacion[ variacion ].etiqueta = temp_word;
							else
								double_X_slider[ cursor[ Coordinates::Y ] - 1 ].leave_cursor();
							double_X_slider[ cursor[ Coordinates::Y ] ].set_cursor_at_left();
							break;
						case 4:
							// Análogo a case 3
							if ( cursor[ Coordinates::Y ] == 0 )
								info->variacion[ variacion ].etiqueta = temp_word;
							else
								double_X_slider[ cursor[ Coordinates::Y ] - 1 ].leave_cursor();
							double_X_slider[ cursor[ Coordinates::Y ] ].set_cursor_at_right();
							break;
					}
					will_dump = true;
				}
				break;/*}}}*/

			case KEY_UP :/*{{{*/
				if ( cursor[ Coordinates::Y ] > -1 ) { // aún no está en la etiqueta
					--cursor[ Coordinates::Y ];
					switch ( cursor[ Coordinates::X ] ) {
						case 0: // check: nada que salvar
							status_field[ cursor[ Coordinates::Y ] + 1 ].leave_cursor();
							if ( cursor[ Coordinates::Y ] == -1 ) {
								temp_word = info->variacion[ variacion ].etiqueta;
								etiqueta_field.set_cursor();
							}
							else
								status_field[ cursor[ Coordinates::Y ] ].set_cursor();
							break;

						case 1: // instrumento: se salva siempre
							info->instrumento[ cursor[Y] + 1 ] = temp_word;
							if ( cursor[ Coordinates::Y ] == -1 ) {
								temp_word = info->variacion[ variacion ].etiqueta;
								etiqueta_field.set_cursor();
							}
							else {
								temp_word = info->instrumento[ cursor[Y] ];
								instrument_field[ cursor[ Coordinates::Y ] ].set_cursor();
							}
							break;
						case 2: // transposition
							info->variacion[ variacion ].track[ cursor[Y] + 1 ].transposition =
								std::stoi( temp_word );
							if ( cursor[ Coordinates::Y ] == -1 ) {
								temp_word = info->variacion[ variacion ].etiqueta;
								etiqueta_field.set_cursor();
							}
							else {
								temp_word = 
									info->variacion[variacion].track[ cursor[Y] ].transposition == 0
									? "0" : std::to_string(
									info->variacion[ variacion ].track[ cursor[Y] ].transposition );
								transposition_field[ cursor[ Coordinates::Y ] ].set_cursor();
							}
							break;
						case 3:
							double_X_slider[ cursor[ Coordinates::Y ] + 1 ].leave_cursor();
							if ( cursor[ Coordinates::Y ] == -1 ) {
								temp_word = info->variacion[ variacion ].etiqueta;
								etiqueta_field.set_cursor();
							}
							else
								double_X_slider[ cursor[Coordinates::Y] ].set_cursor_at_left();
							break;
						case 4:
							double_X_slider[ cursor[ Coordinates::Y] + 1 ].leave_cursor();
							if ( cursor[ Coordinates::Y ] == -1 ) {
								temp_word = info->variacion[ variacion ].etiqueta;
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
				if ( cursor[ Coordinates::Y ] == -1 ) {
					temp_word.append( " " );
					etiqueta_field.set_content( temp_word );
				}
				else if ( cursor[X] == 0 ) { // CAmbio de status: se guarda
					if ( info->variacion[ variacion ].track[ cursor[Y] ].status == ON ) {
						info->variacion[ variacion ].track[ cursor[Y] ].status = OFF;
						status_field[ cursor[Y] ].off();
						instrument_field[ cursor[Y] ].off();
						transposition_field[ cursor[ Y ] ].off();
						double_X_slider[ cursor[ Y ] ].off();
					}
					else {
						info->variacion[ variacion ].track[ cursor[Y] ].status = ON;
						status_field[ cursor[Y] ].on();
						instrument_field[ cursor[Y] ].on();
						transposition_field[ cursor[ Y ] ].on();
						double_X_slider[ cursor[ Y ] ].on();
				}
				instrument_field[ cursor[Y] ].set_text( info->instrumento[ cursor[Y] ] );
				transposition_field[ cursor[Y] ].set_value(
						info->variacion[ variacion ].track[ cursor[Y] ].transposition );
				}
				else if ( cursor[X] == 1 ) {
					temp_word.append( " " );
					instrument_field[ cursor[Y] ].set_text( temp_word );
				}
				will_dump = true;

				break;/*}}}*/

			case KEY_BACKSPACE :/*{{{*/
				if ( cursor[ Coordinates::Y ] == -1 ) {
					temp_word = temp_word.substr( 0, temp_word.length() - 1 );
					etiqueta_field.set_content( temp_word );
				}
				else if ( cursor[X] == 1 ) {
					temp_word = temp_word.substr( 0, temp_word.length() - 1 );
					instrument_field[ cursor[Y] ].set_text( temp_word );
				}
				else if ( cursor[X] == 2 ) {
					temp_word = temp_word.substr( 0, temp_word.length() - 1 );
					transposition_field[ cursor[Y] ].set_text( temp_word );
				}
				will_dump = true;

				break;/*}}}*/

			case 546 : // CTRL-KEY_LEFT{{{
				if ( cursor[Y] > -1 ) { // Zona de controles
					if ( cursor[X] == 3 ) { // left slider
						if ( double_X_slider[ cursor[Y] ].decrease_left_slider() == Moved::YES )
							--( info->variacion[ variacion ].track[ cursor[Y] ].lower_key );
					}
					else if ( cursor[X] == 4 ) { // right slider
						if ( double_X_slider[ cursor[Y] ].decrease_right_slider() == Moved::YES )
							--( info->variacion[ variacion ].track[ cursor[Y] ].upper_key );
					}
					will_dump = true;
				}

				break;/*}}}*/

			// CTRL-KEY_RIGHT{{{
			case 561 :
				if ( cursor[Y] > -1 ) {
					if ( cursor[X] == 3 ) {
						if ( double_X_slider[ cursor[Y] ].increase_left_slider() == Moved::YES )
							++( info->variacion[ variacion ].track[ cursor[Y] ].lower_key );
					}
					else if ( cursor[X] == 4 ) {
						if ( double_X_slider[ cursor[Y] ].increase_right_slider() == Moved::YES )
							++( info->variacion[ variacion ].track[ cursor[Y] ].upper_key );
					}
					will_dump = true;
				}
				break;/*}}}*/

			case '\n' :/*{{{*/
				if ( cursor[Y] == -1 )
					info->variacion[ variacion ].etiqueta = temp_word;
				else switch ( cursor[X] ) {
					case 1 :
						info->instrumento[ cursor[Y] ] = temp_word;
						break;
					case 2 :
						info->variacion[ variacion ].track[ cursor[Y] ].transposition =
							temp_word == "" ? 0 : std::stoi( temp_word );
						break;
					default:
						break;
				 }

				// Dumpeamos sí o sí
				if ( keyboard->get_MIDI_state() == Switch::ON ) {
					keyboard->select_page( Page::TIMBRE );
					keyboard->dump_variation( *info, variacion );
				}

				break;/*}}}*/

			case 27 :/*{{{*/
				curs_set( false );
				hide();
				update_popups();
				again = false;

				will_dump = false;

				break;/*}}}*/

			case 1 : // DUplicate{{{
				if ( info->n_variaciones < 16 ) {
					for ( int32_t i = info->n_variaciones - 1; i >= variacion; --i )
						info->variacion[ i + 1 ] = info->variacion[ i ];
					++info->n_variaciones;
					++variacion;
					update();
					if ( cursor[Y] == -1 ) { // si está hasta arriba
						info->variacion[ variacion + 1 ].etiqueta = temp_word; // guardamos
						temp_word = info->variacion[ variacion ].etiqueta; // copiamos nuevo
						etiqueta_field.set_cursor();
					}
					else
						switch ( cursor[X] ) {
							case 0:
								status_field[ cursor[Y] ].set_cursor();
								break;
							case 1:
								info->instrumento[ cursor[Y] ] = temp_word;
								instrument_field[ cursor[Y] ].set_cursor();
								break;
							case 2:
								info->variacion[ variacion + 1 ].track[ cursor[Y] ].transposition =
									std::stoi( temp_word );
								temp_word = 
									info->variacion[variacion].track[ cursor[Y] ].transposition == 0
									? "0" : std::to_string(
									info->variacion[ variacion ].track[ cursor[Y] ].transposition );
								transposition_field[ cursor[Y] ].set_cursor();
								break;
							case 3:
								double_X_slider[ cursor[Coordinates::Y] ].set_cursor_at_left();
								break;
							case 4:
								double_X_slider[ cursor[Coordinates::Y ] ].set_cursor_at_right();
								break;
						}
					if ( keyboard->get_MIDI_state() == Switch::ON ) {
						keyboard->select_page( Page::TIMBRE );
						keyboard->dump_variation( *info, variacion );
					}
					will_dump = false;
				}
				break;/*}}}*/

			case 18 : // Eliminate{{{
				if ( info->n_variaciones > 1 ) {
					if ( variacion == info->n_variaciones - 1 )
						--variacion;
					for ( int32_t i = variacion + 1; i < info->n_variaciones; ++i )
						info->variacion[ i + 1 ] = info->variacion[ i ];
					--info->n_variaciones;
					update();
					if ( cursor[Y] == -1 ) { // si está hasta arriba
						info->variacion[ variacion + 1 ].etiqueta = temp_word; // guardamos
						temp_word = info->variacion[ variacion ].etiqueta; // copiamos nuevo
						etiqueta_field.set_cursor();
					}
					else
						switch ( cursor[X] ) {
							case 0:
								status_field[ cursor[Y] ].set_cursor();
								break;
							case 1:
								info->instrumento[ cursor[Y] ] = temp_word;
								instrument_field[ cursor[Y] ].set_cursor();
								break;
							case 2:
								info->variacion[ variacion + 1 ].track[ cursor[Y] ].transposition =
									std::stoi( temp_word );
								temp_word = 
									info->variacion[variacion].track[ cursor[Y] ].transposition == 0
									? "0" : std::to_string(
									info->variacion[ variacion ].track[ cursor[Y] ].transposition );
								transposition_field[ cursor[Y] ].set_cursor();
								break;
							case 3:
								double_X_slider[ cursor[Coordinates::Y] ].set_cursor_at_left();
								break;
							case 4:
								double_X_slider[ cursor[Coordinates::Y ] ].set_cursor_at_right();
								break;
						}
					if ( keyboard->get_MIDI_state() == Switch::ON ) {
						keyboard->select_page( Page::TIMBRE );
						keyboard->dump_variation( *info, variacion );
					}
					will_dump = false;
				}
				break;/*}}}*/

			default :/*{{{*/
				if ( tecla == 32 or ( 44 <= tecla and tecla <= 122 ) or tecla == 263 ) {
					if ( cursor[Y] == -1 ) {
						sprintf( tecla_c_string, "%c", tecla );
						temp_word = temp_word + tecla_c_string;
						etiqueta_field.set_content( temp_word );
					}
					else switch ( cursor[X] ) {
						case 1:
							sprintf( tecla_c_string, "%c", tecla );
							temp_word = temp_word + tecla_c_string;
							instrument_field[ cursor[Y] ].set_text( temp_word );
							break;
						case 2 :
							sprintf( tecla_c_string, "%c", tecla );
							temp_word = temp_word + tecla_c_string;
							transposition_field[ cursor[Y] ].set_text( temp_word );
							break;
						default:
							break;
					}
				will_dump = false;
				break;/*}}}*/
				}
		}
		/* Por ahora eliminamos esta parte, vamos a reservar el dumping a la tecla ENTER
		 * y a los cambios de variación. Y probaremos cómo se siente!!
		 */
		if ( will_dump and keyboard->get_MIDI_state() == Switch::ON ) {
			// keyboard->select_page( Page::TIMBRE );
			// keyboard->dump_variation( *info, variacion );
		}

	} while ( again );
}

void Orchestra::reset_variation() noexcept/*{{{*/
{
	variacion = 0;
}/*}}}*/
