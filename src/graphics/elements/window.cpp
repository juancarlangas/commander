#include "graphics/elements/window.hpp"
#include <bits/stdint-intn.h>
#include <cstdlib>
#include <curses.h>
#include <iostream>
#include <ncurses.h>

void Window::init(	const int32_t _Ysize, const int32_t _Xsize,
					const int32_t _Ypos, const int32_t _Xpos ) noexcept
{
	y_size = _Ysize;
	x_size = _Xsize;
	y_pos = _Ypos;
	x_pos = _Xpos;

	area = newwin( y_size, x_size, y_pos, x_pos );
}

void Window::init( const int32_t _Ysize, const int32_t _Xsize,
		const int32_t _Ypos, const int32_t _Xpos,
		const struct Font &_NativeFont, const struct Font &_CursorFont,
		const struct Font &_DimmedFont ) noexcept
{
	init( _Ysize, _Xsize, _Ypos, _Xpos );

	inherit_font = _NativeFont;
	cursor_font = _CursorFont;
	dimmed_font = _DimmedFont;
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

void Window::set_font_reverse( const bool &_Is_Reverse ) noexcept
{
	if ( _Is_Reverse )
		wattron( area, A_REVERSE );
	else
		wattroff( area, A_REVERSE );
}

void Window::set_font_blinking( const bool &_Is_Blinking ) noexcept
{
	_Is_Blinking ? wattron( area, A_BLINK ) : wattroff( area, A_BLINK );
}

void Window::update() noexcept
{
	wrefresh( area );
}
