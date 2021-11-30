#include "graphics/orchestra.hpp"
#include "elements/windows/popups/text_popup.hpp"
#include "elements/windows/popups/double_X_slider.hpp"
#include "graphics/colors.hpp"
#include "graphics/ncurses.hpp"
#include <ncurses.h>
#include <panel.h>
#include <string>
#include <iostream>

Orchestra::Orchestra() :
	native_font { { { GREEN_DEFAULT, "Bold" },
					{ BLUE_DEFAULT, "Bold" },
					{ YELLOW_DEFAULT, "Bold" },
					{ BLUE_DEFAULT, "Regular" },
					{ MAGENTA_DEFAULT, "Regular" },
					{ YELLOW_DEFAULT, "Regular" },
					{ CYAN_DEFAULT, "Bold" },
					{ RED_DEFAULT, "Bold " } } },
	cursor_font { WHITE_DEFAULT, "Bold" },
	dimmed_font { GRAY_DEFAULT, "Bold" },
	cursor_map { { { false } } }
{}

void Orchestra::init( const int32_t _Ysize, const int32_t _Xsize,
						const int32_t _Ypos, const int32_t _Xpos ) noexcept
{
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

		transposition_field[i].init( 1, 3, y_starting_point + i, _Xpos + 39,
				native_font[ i ], cursor_font, dimmed_font );
		transposition_field[i].update();
		
		// invocamos su clase base porque no cuadra con su init
		double_X_slider[i].Popup::init( 1, 62, y_starting_point + i, _Xpos + 44,
				native_font[ i ], cursor_font, dimmed_font );
		double_X_slider[i].update();
	}

	hide();

	cursor_map[ 0 ][ 0 ] = true;
}

void Orchestra::show( struct System *&_Row ) noexcept
{
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
}

void Orchestra::update() noexcept
{
	variacion_text_box.set_text( "Variacion " + std::to_string( variacion + 1 ) +
								" de " + std::to_string( info->n_variaciones ) );
	etiqueta_field.set_content( info->variacion[ variacion ].etiqueta );

	for ( int32_t i = 0; i < 8; ++i ) {
		bool track_is_active = info->variacion[ variacion ].track[ i ].status;

		int16_t &l_Value = info->variacion[ variacion ].track[ i ].lower_key;
		int16_t &r_Value = info->variacion[ variacion ].track[ i ].upper_key;
		if ( l_Value > r_Value )
			r_Value = l_Value;
		if ( l_Value < MIN_KEY )
			l_Value = MIN_KEY;
		if ( r_Value > MAX_KEY )
			r_Value = MAX_KEY;
		double_X_slider[i].set_values( l_Value, r_Value );

		if ( track_is_active ) {
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
		transposition_field[ i ].set_value( info->variacion[ variacion ].track[ i ].transposition );
	}
}

void Orchestra::capture_key() noexcept
{
	cursor[ Coordinates::X ] = 0;
	cursor[ Coordinates::Y ] = 0;

	curs_set( true );
	etiqueta_field.set_cursor();
	int32_t tecla;
	bool again = true;

	/* Como el primer track representa [0] pero le corresponde el cursor[y] = 1, es que
	 * jugamos con las restas -1 y -2 */

	do {
		switch ( tecla = getch() ) {
			// LEFT
			case KEY_LEFT :
				if ( variacion > 0 ) {
					--variacion;
					update();
					if ( cursor[ Coordinates::Y ] == 0 ) // si está hasta arriba
						etiqueta_field.set_cursor();
					else
						switch ( cursor[ Coordinates::X ] ) {
							case 0:
								status_field[ cursor[ Coordinates::Y ] - 1 ].set_cursor();
								break;
							case 1:
								instrument_field[ cursor[ Coordinates::Y ] - 1 ].set_cursor();
								break;
							case 2:
								transposition_field[ cursor[ Coordinates::Y ] - 1 ].set_cursor();
								break;
							case 3:
								double_X_slider[ cursor[Coordinates::Y ] - 1 ].set_cursor_at_left();
								break;
							case 4:
								double_X_slider[ cursor[Coordinates::Y ] - 1 ].set_cursor_at_right();
								break;
						}
				}
				break;

			// RIGHT
			case KEY_RIGHT :
				if ( variacion < info->n_variaciones - 1 ) {
					++variacion;
					update();
					if ( cursor[ Coordinates::Y ] == 0 )
						etiqueta_field.set_cursor();
					else
						switch ( cursor[ Coordinates::X ] ) {
							case 0:
								status_field[ cursor[ Coordinates::Y ] - 1 ].set_cursor();
								break;
							case 1:
								instrument_field[ cursor[ Coordinates::Y ] - 1 ].set_cursor();
								break;
							case 2:
								transposition_field[ cursor[ Coordinates::Y ] - 1 ].set_cursor();
								break;
							case 3:
								double_X_slider[ cursor[Coordinates::Y ] - 1].set_cursor_at_left();
								break;
							case 4:
								double_X_slider[ cursor[Coordinates::Y ] - 1].set_cursor_at_right();
								break;
						}
				}
				break;

			// KEY_DOWN
			case KEY_DOWN :
				if ( cursor[ Coordinates::Y ] < 8 ) {
					++cursor[ Coordinates::Y ];
					switch ( cursor[ Coordinates::X ] ) {
						case 0:
							if ( cursor[ Coordinates::Y ] > 1 )
								status_field[ cursor[ Coordinates::Y ] - 2 ].leave_cursor();
							status_field[ cursor[ Coordinates::Y ] - 1 ].set_cursor();
							break;
						case 1:
							instrument_field[ cursor[ Coordinates::Y ] - 1 ].set_cursor();
							break;
						case 2:
							transposition_field[ cursor[ Coordinates::Y ] - 1 ].set_cursor();
							break;
						case 3:
							// Si el cursor eje Y es 1, estará debajo de la etiqueta y no existirá
							// cursor[ Coordinates::Y ] - 2 por lo que habrá violación de acceso
							if ( cursor[ Coordinates::Y ] > 1 )
								double_X_slider[ cursor[ Coordinates::Y ] - 2 ].leave_cursor();
							double_X_slider[ cursor[ Coordinates::Y ] - 1 ].set_cursor_at_left();
							break;
						case 4:
							// Análogo a case 3
							if ( cursor[ Coordinates::Y ] > 1 )
								double_X_slider[ cursor[ Coordinates::Y ] - 2 ].leave_cursor();
							double_X_slider[ cursor[ Coordinates::Y ] - 1 ].set_cursor_at_right();
							break;
					}
					break;
				}
				break;

			case KEY_UP :
				if ( cursor[ Coordinates::Y ] > 0 ) {
					--cursor[ Coordinates::Y ];
					switch ( cursor[ Coordinates::X ] ) {
						case 0:
							status_field[ cursor[ Coordinates::Y ] ].leave_cursor();
							if ( cursor[ Coordinates::Y ] == 0 )
								etiqueta_field.set_cursor();
							else
								status_field[ cursor[ Coordinates::Y ] - 1 ].set_cursor();
							break;
						case 1:
							if ( cursor[ Coordinates::Y ] == 0 )
								etiqueta_field.set_cursor();
							else
								status_field[ cursor[ Coordinates::Y ] - 1 ].set_cursor();
							break;
						case 2:
							if ( cursor[ Coordinates::Y ] == 0 )
								etiqueta_field.set_cursor();
							else
								transposition_field[ cursor[ Coordinates::Y] - 1 ].set_cursor();
							break;
						case 3:
							double_X_slider[ cursor[ Coordinates::Y ] ].leave_cursor();
							if ( cursor[ Coordinates::Y ] == 0 )
								etiqueta_field.set_cursor();
							else
								double_X_slider[ cursor[Coordinates::Y] - 1 ].set_cursor_at_left();
							break;
						case 4:
							double_X_slider[ cursor[ Coordinates::Y] ].leave_cursor();
							if ( cursor[ Coordinates::Y ] == 0 )
								etiqueta_field.set_cursor();
							else
								double_X_slider[ cursor[Coordinates::Y] - 1 ].set_cursor_at_right();
							break;
					}
					break;
				}
				break;

			// Shift + TAB
			case 353 :
				if ( cursor[ Coordinates::X ] > 0 ) {
					--cursor[ Coordinates::X ];
					if ( cursor[ Coordinates::Y ] > 0 )
						switch ( cursor[ Coordinates::X ] ) {
							case 0 :
								status_field[ cursor[ Coordinates::Y ] - 1 ].set_cursor();
								break;
							case 1 :
								instrument_field[ cursor[ Coordinates::Y ] - 1 ].set_cursor();
								break;
							case 2 :
								double_X_slider[ cursor[ Coordinates::Y ] - 1 ].leave_cursor();
								transposition_field[ cursor[ Coordinates::Y ] - 1 ].set_cursor();
								break;
							case 3 :
								double_X_slider[ cursor[ Coordinates::Y ] - 1 ].swap_cursor();
								break;
						}
				break;
				}
				break;

			// TAB
			case 9 :
				if ( cursor[ Coordinates::X ] < 4 ) {
					++cursor[ Coordinates::X ];
					if ( cursor[ Coordinates::Y ] > 0 )
						switch ( cursor[ Coordinates::X ] ) {
							case 1 :
								status_field[ cursor[ Coordinates::Y ] - 1 ].leave_cursor();
								instrument_field[ cursor[ Coordinates::Y ] - 1 ].set_cursor();
								break;
							case 2 :
								transposition_field[ cursor[ Coordinates::Y ] - 1 ].set_cursor();
								break;
							case 3 :
								double_X_slider[ cursor[ Coordinates::Y ] - 1 ].set_cursor_at_left();
								break;
							case 4 :
								double_X_slider[ cursor[ Coordinates::Y ] - 1 ].swap_cursor();
								break;
						}
				}
				break;

			// DEFAULT
			 // Esc
			case 27 :
				curs_set( false );
				hide();
				update_popups();
				again = false;
				break;
			default :
				if ( tecla == 32 or ( 44 <= tecla and tecla <= 122 ) or tecla == 263 )
				break;
		}
	} while ( again );
}

void KeyboardScheme::auto_draw() noexcept
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
}

void Orchestra::hide() noexcept
{
	etiqueta_field.hide();
	variacion_text_box.hide();
	keyboard_scheme.hide();
	base.hide();
}
