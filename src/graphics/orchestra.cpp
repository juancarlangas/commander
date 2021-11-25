#include "graphics/orchestra.hpp"
#include "elements/text_popup.hpp"
#include "graphics/colors.hpp"
#include "graphics/ncurses.hpp"
#include <ncurses.h>
#include <panel.h>
#include <string>

void KeyboardScheme::init(	const int32_t _Ysize, const int32_t _Xsize,
							const int32_t _Ypos, const int32_t _Xpos ) noexcept
{
	TextPopup::init( 6, 64, _Ypos + ( _Ysize * 50 / 200 ), _Xpos + ( _Xsize / 2 ) - 31 );
	TextPopup::set_font_color( WHITE_DEFAULT );
	TextPopup::set_font_width( "Bold" );
	TextPopup::update();
}

void Orchestra::init( const int32_t _Ysize, const int32_t _Xsize,
						const int32_t _Ypos, const int32_t _Xpos ) noexcept
{
	variacion = 0;

	// Base
	base.init( _Ysize, _Xsize, _Ypos, _Xpos );
	base.set_box_color( WHITE_DEFAULT );
	base.set_font_width( "Bold" );
	base.define_box( 0, 0, 0, 0, 0, 0, 0, 0 );
	base.update();

	// Indicador de variación
	variacion_text_box.init( 3, 20, _Ypos + 1, _Xpos + 2 );
	variacion_text_box.set_font_color( GRAY_DEFAULT );
	variacion_text_box.set_font_width( "Bold" );
	variacion_text_box.update();

	// keyboard_text_box
	keyboard_scheme.init( 6, 64, _Ysize * 50 / 200, _Xpos + ( _Xsize / 2 ) - 31 );


	// Campos
	etiqueta_field.create( "Etiqueta", 3, _Xsize * 80 / 200, _Ypos + 1, _Xpos + ( _Xsize / 4 ),
							STRING, 16 );
	etiqueta_field.set_font_width( "Bold" );
	etiqueta_field.set_font_color( WHITE_DEFAULT );

	hide();
}

void Orchestra::show( struct System *&_Row ) noexcept
{
	info = _Row;

	base.show();

	variacion_text_box.show();
	variacion_text_box.set_text( "Variacion " + std::to_string( variacion + 1 ) +
								" de " + std::to_string( info->n_variaciones ) );

	keyboard_scheme.show();

	etiqueta_field.show();
	etiqueta_field.set_content( info->variacion[ variacion ].etiqueta );
}

void Orchestra::hide() noexcept
{
	etiqueta_field.hide();
	variacion_text_box.hide();
	keyboard_scheme.hide();
	base.hide();
}

void Orchestra::capture_key() noexcept
{
	curs_set( true );
	etiqueta_field.get_cursor();
	int32_t tecla;
	bool again = true;

	do {
		switch ( tecla = getch() ) {
			 // Esc
			case 27 :
				curs_set( false );
				hide();
				update_popups();
				again = false;
				break;
			// LEFT
			case KEY_LEFT :
				if ( variacion > 0 ) {
					--variacion;
					variacion_text_box.set_text( "Variacion " + std::to_string( variacion + 1 ) +
												" de " + std::to_string( info->n_variaciones ) );
					etiqueta_field.set_content( info->variacion[ variacion ].etiqueta );
				}
				break;
			// RITHG
			case KEY_RIGHT :
				if ( variacion < info->n_variaciones - 1 ) {
					++variacion;
					variacion_text_box.set_text( "Variacion " + std::to_string( variacion + 1 ) +
												" de " + std::to_string( info->n_variaciones ) );
					etiqueta_field.set_content( info->variacion[ variacion ].etiqueta );
				}
				break;
			// DEFAULT
			default :
				if ( tecla == 32 or ( 44 <= tecla and tecla <= 122 ) or tecla == 263 )
				break;
		}
	} while ( again );
}
