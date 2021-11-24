#include "graphics/popup.hpp"
#include "ventana.hpp"
#include <cstdlib>
#include <iostream>

void Popup::init( const int32_t _Ysize, const int32_t _Xsize,
					const int32_t _Ypos, const int32_t _Xpos ) noexcept
{
	Ventana::init( _Ysize, _Xsize, _Ypos, _Xpos );
	panel = new_panel( ventana );
}

void Popup::show() noexcept
{
	show_panel( panel );
}

void Popup::hide() noexcept
{
	hide_panel( panel );
}
