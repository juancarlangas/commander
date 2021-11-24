#include "graphics/ventana.hpp"
#include <cstdlib>
#include <iostream>

void Ventana::init(	const int32_t _Ysize, const int32_t _Xsize,
					const int32_t _Ypos, const int32_t _Xpos ) noexcept
{
	y_size = _Ysize;
	x_size = _Xsize;
	y_pos = _Ypos;
	x_pos = _Xpos;

	ventana = newwin( y_size, x_size, y_pos, x_pos );
}

void Ventana::set_font_color( const int32_t &_Color ) noexcept
{
	wattron( ventana, COLOR_PAIR( _Color ) );
}

void Ventana::set_font_width( const std::string_view _Style ) noexcept
{
	if ( _Style != "Bold" and _Style != "Regular" ) {
		std::cerr << _Style << " no se reconoce como un estilo en Ventana::set_font_width()"
			<< std::endl;
		exit( EXIT_FAILURE );
	}

	if ( _Style == "Regular" )
		wattroff( ventana, A_BOLD );
	else
		wattron( ventana, A_BOLD );
}

void Ventana::set_border_symbols(	const int32_t &_Top, const int32_t &_Right,
								const int32_t &_Bottom, const int32_t &_Left,
								const int32_t &_TopLeft, const int32_t &_TopRight,
								const int32_t &_BottomRight, const int32_t &_BottomLeft ) noexcept
{
	wborder( ventana, _Top, _Right, _Bottom, _Left, _TopLeft, _TopRight, _BottomRight, _BottomLeft);
}

void Ventana::draw() noexcept
{
	wrefresh( ventana );
}

void Ventana::update() noexcept
{
	wrefresh( ventana );
}
