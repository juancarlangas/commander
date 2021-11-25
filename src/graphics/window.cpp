#include "graphics/window.hpp"
#include <bits/stdint-intn.h>
#include <cstdlib>
#include <curses.h>
#include <iostream>

void Window::init(	const int32_t _Ysize, const int32_t _Xsize,
					const int32_t _Ypos, const int32_t _Xpos ) noexcept
{
	y_size = _Ysize;
	x_size = _Xsize;
	y_pos = _Ypos;
	x_pos = _Xpos;

	box = newwin( y_size, x_size, y_pos, x_pos );
	area = newwin( y_size - 2, x_size - 2, y_pos + 1, x_pos + 1 );
}

void Popup::init(	const int32_t _Ysize, const int32_t _Xsize,
					const int32_t _Ypos, const int32_t _Xpos ) noexcept
{
	Window::init( _Ysize, _Xsize, _Ypos, _Xpos );
	box_panel = new_panel( box );
	area_panel = new_panel( area );
}

void Window::set_box_color( const int32_t &_Color ) noexcept
{
	wattron( box, COLOR_PAIR( _Color ) );
}

void Window::set_font_color( const int32_t &_Color ) noexcept
{
	wattron( area, COLOR_PAIR( _Color ) );
}

void Window::set_box_width( const std::string_view _Style ) noexcept
{
	if ( _Style != "Bold" and _Style != "Regular" ) {
		std::cerr << _Style << " no se reconoce como un estilo en Window::set_font_width()"
			<< std::endl;
		exit( EXIT_FAILURE );
	}

	if ( _Style == "Regular" )
		wattroff( box, A_BOLD );
	else
		wattron( box, A_BOLD );
}

void Window::set_font_width( const std::string_view _Style ) noexcept
{
	if ( _Style != "Bold" and _Style != "Regular" ) {
		std::cerr << _Style << " no se reconoce como un estilo en Window::set_font_width()"
			<< std::endl;
		exit( EXIT_FAILURE );
	}

	if ( _Style == "Regular" )
		wattroff( area, A_BOLD );
	else
		wattron( area, A_BOLD );
}

void Window::define_box(	const int32_t &_Top, const int32_t &_Right,
							const int32_t &_Bottom, const int32_t &_Left,
							const int32_t &_TopLeft, const int32_t &_TopRight,
							const int32_t &_BottomRight, const int32_t &_BottomLeft ) noexcept
{
	wborder( box, _Top, _Right, _Bottom, _Left, _TopLeft, _TopRight, _BottomRight, _BottomLeft);
}

void Window::draw() noexcept
{
	wrefresh( box );
	wrefresh( area );
}

void Window::update() noexcept
{
	wrefresh( box );
	wrefresh( area );
}

void Popup::show() noexcept
{
	show_panel( box_panel );
	show_panel( area_panel );
}

void Popup::hide() noexcept
{
	hide_panel( area_panel );
	hide_panel( box_panel );
}
