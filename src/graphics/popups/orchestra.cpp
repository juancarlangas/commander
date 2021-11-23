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
	base.set_color( WHITE_DEFAULT );
	base.set_font_width( "Bold" );
	base.set_borders( 0, 0, 0, 0, 0, 0, 0, 0 );
	base.draw();
	base.hide();

	// Campos
	etiqueta_field.create( "Etiqueta", 3, _Xsize * 80 / 200, _Ypos + 1, _Xpos + ( _Xsize / 4 ),
							STRING, 16 );
	etiqueta_field.hide();

	update_panels();
	doupdate();
}

void Orchestra::show() noexcept
{
	base.show();
	etiqueta_field.show();
}

void Orchestra::hide() noexcept
{
	etiqueta_field.hide();
	base.hide();
}

void Orchestra::capture_key() noexcept
{
	int32_t tecla = getch();

	do {
		switch ( tecla ) {
			case 27 :
				hide();
				update_popups();
				break;
			default :
				break;
		}
	} while ( tecla != 27 );
}
