#include "graphics/popups/orchestra.hpp"
#include "graphics/colors.hpp"
#include "graphics/ncurses.hpp"
#include <ncurses.h>
#include <panel.h>

void Orchestra::init(	const int32_t _Ysize, const int32_t _Xsize,
						const int32_t _Ypos, const int32_t _Xpos ) noexcept
{
	// Base
	base.init( _Ysize, _Xsize, _Ypos, _Xpos );
	base.set_border_color( WHITE_DEFAULT );
	base.set_border_font_width( "Bold" );
	base.set_border_symbols( 0, 0, 0, 0, 0, 0, 0, 0 );
	base.draw();
	base.hide();

	// Campos
	etiqueta_field.create( "Etiqueta", 3, _Xsize * 80 / 200, _Ypos + 1, _Xpos + ( _Xsize / 4 ),
							STRING, 16 );
	etiqueta_field.hide();

	variacion = 0;

	update_panels();
	doupdate();
}

void Orchestra::show( struct System *&_Row ) noexcept
{
	info = _Row;
	base.show();
	etiqueta_field.show();
	etiqueta_field.set_content( info->variacion[ variacion ].etiqueta );
}

void Orchestra::hide() noexcept
{
	etiqueta_field.hide();
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
				if ( variacion > 0 )
					--variacion;
				etiqueta_field.set_content( info->variacion[ variacion ].etiqueta );
				break;
			// RITHG
			case KEY_RIGHT :
				if ( variacion < info->n_variaciones - 1 )
					++variacion;
				etiqueta_field.set_content( info->variacion[ variacion ].etiqueta );
				break;
			// DEFAULT
			default :
				if ( tecla == 32 or ( 44 <= tecla and tecla <= 122 ) or tecla == 263 )
				break;
		}
	} while ( again );
}
