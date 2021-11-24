#include "graphics/popup.hpp"
#include <cstdlib>
#include <iostream>

void Popup::init( const int32_t _Ysize, const int32_t _Xsize,
					const int32_t _Ypos, const int32_t _Xpos ) noexcept
{
	y_size = _Ysize;
	x_size = _Xsize;
	y_pos = _Ypos;
	x_pos = _Xpos;

	win_box = newwin( y_size, x_size, y_pos, x_pos );
	panel_box = new_panel( win_box );

	win_area = newwin( y_size - 2, x_size - 2, y_pos - 2, x_pos - 2 );
	panel_area = new_panel( win_area );
}

void Popup::set_border_color( const int32_t &_Color ) noexcept
{
	wattron( win_box, COLOR_PAIR( _Color ) );
}

void Popup::set_border_font_width( const std::string_view _Style )
{
	if ( _Style != "Bold" and _Style != "Regular" ) {
		std::cerr << _Style << " no se reconoce como un estilo en Popup::set_font_width()"
			<< std::endl;
		exit( EXIT_FAILURE );
	}

	if ( _Style == "Regular" )
		wattroff( win_box, A_BOLD );
	else
		wattron( win_box, A_BOLD );
}

void Popup::set_border_symbols(	const int32_t &_Top, const int32_t &_Right,
								const int32_t &_Bottom, const int32_t &_Left,
								const int32_t &_TopLeft, const int32_t &_TopRight,
								const int32_t &_BottomRight, const int32_t &_BottomLeft ) noexcept
{
	wborder( win_box, _Top, _Right, _Bottom, _Left, _TopLeft, _TopRight, _BottomRight, _BottomLeft);
}

void Popup::draw() noexcept
{
	wrefresh( win_box );
	wrefresh( win_area );
}

void Popup::update() noexcept
{
	wrefresh( win_box );
	wrefresh( win_area );
}

void Popup::show() noexcept
{
	show_panel( panel_box );
	show_panel( panel_area );
}

void Popup::hide() noexcept
{
	hide_panel( panel_area );
	hide_panel( panel_box );
}
