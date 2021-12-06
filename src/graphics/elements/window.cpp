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


void BoxedWindow::init(	const int32_t _Ysize, const int32_t _Xsize,
						const int32_t _Ypos, const int32_t _Xpos,
						const int32_t _Frame_Width ) noexcept
{
	box = newwin( _Ysize, _Xsize, _Ypos, _Xpos );
	Window::init( _Ysize - _Frame_Width * 2, _Xsize - _Frame_Width * 2,
					_Ypos - _Frame_Width, _Xpos - _Frame_Width + 1 );
}


void BoxedWindow::set_box_color( const int32_t &_Color ) noexcept
{
	wattron( box, COLOR_PAIR( _Color ) );
}

void Window::set_font_color( const int32_t &_Color ) noexcept
{
	wattron( area, COLOR_PAIR( _Color ) );
}

void BoxedWindow::set_box_width( const std::string_view _Style ) noexcept
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

void BoxedWindow::define_box(	const int32_t &_Top, const int32_t &_Right,
							const int32_t &_Bottom, const int32_t &_Left,
							const int32_t &_TopLeft, const int32_t &_TopRight,
							const int32_t &_BottomRight, const int32_t &_BottomLeft ) noexcept
{
	wborder( box, _Top, _Right, _Bottom, _Left, _TopLeft, _TopRight, _BottomRight, _BottomLeft);
}

void BoxedWindow::draw() noexcept
{
	wrefresh( box );
	wrefresh( area );
}

void Window::update() noexcept
{
	wrefresh( area );
}

void BoxedWindow::update() noexcept
{
	wrefresh( box );
	Window::update();
}
