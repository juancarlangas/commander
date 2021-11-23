#include "graphics/popups/orchestra.hpp"

void Orchestra::init(	const int32_t _Ysize, const int32_t _Xsize,
						const int32_t _Xpos, const int32_t _Ypos ) noexcept
{
	y_size = _Ysize;
	x_size = _Xsize;
	y_pos = _Ypos;
	x_pos = _Xpos;

	ventana = newwin( y_size, x_size, y_pos, x_pos );
	panel = new_panel( ventana );
	// los campos (fields) sí pueden ser creados en el consructor de Orchestra porque
	// realmente serán creados hasta la instanciación, así que en teoría da tiempo
	etiqueta_field.create( "Etiqueta", 3, 16, 1, 20, STRING, 16 );
}
