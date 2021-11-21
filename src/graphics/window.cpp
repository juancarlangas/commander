#include "graphics/window.hpp"

Ventana::Ventana() {}
Ventana::~Ventana() {}

void Ventana::init( const int32_t _Ysize, const int32_t _Xsize,
					const int32_t _Xpos, const int32_t _Ypos ) noexcept
{
	box = newwin( _Ysize, _Xsize, _Ypos, _Xpos );
}

void Ventana::set_box_color( const int32_t &_Color ) noexcept
{
	wattron( box, COLOR_PAIR( _Color ) );
}
