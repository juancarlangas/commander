#include "graphics/popups/orchestra.hpp"
#include "graphics/colors.hpp"
#include "graphics/ncurses.hpp"
#include <ncurses.h>
#include <panel.h>
#include <string>

void Orchestra::init(	const int32_t _Ysize, const int32_t _Xsize,
						const int32_t _Ypos, const int32_t _Xpos ) noexcept
{
	variacion = 0;

	// Caja
	box.init( _Ysize, _Xsize, _Ypos, _Xpos );
	box.set_font_color( WHITE_DEFAULT );
	box.set_font_width( "Bold" );
	box.set_border_symbols( 0, 0, 0, 0, 0, 0, 0, 0 );
	box.draw();
	box.hide();

	// Box
	area.init( _Ysize - 2, _Xsize - 2, _Ypos + 1, _Xpos + 1 );
	area.draw();
	area.hide();
	
	// text-box
	variacion_text_box.init( 1, 18, _Ypos + 2, _Xpos + 3 );
	variacion_text_box.set_font_color( GRAY_DEFAULT );
	variacion_text_box.set_font_width( "Bold" );
	variacion_text_box.draw();
	variacion_text_box.hide();

	// Campos
	etiqueta_field.create( "Etiqueta", 3, _Xsize * 80 / 200, _Ypos + 1, _Xpos + ( _Xsize / 4 ),
							STRING, 16 );
	etiqueta_field.set_font_width( "Bold" );
	etiqueta_field.set_font_color( WHITE_DEFAULT );
	etiqueta_field.hide();

	update_panels();
	doupdate();
}

void Orchestra::show( struct System *&_Row ) noexcept
{
	info = _Row;

	box.show();
	area.show();

	variacion_text_box.show();
	variacion_text_box.set_text( "Variacion " + std::to_string( variacion + 1 ) +
								" de " + std::to_string( info->n_variaciones ) );

	etiqueta_field.show();
	etiqueta_field.set_content( info->variacion[ variacion ].etiqueta );
}

void Orchestra::hide() noexcept
{
	etiqueta_field.hide();

	variacion_text_box.hide();
	area.hide();
	box.hide();
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
