#include "double_X_slider.hpp"
#include <cstdlib>
#include <curses.h>
#include <iostream>
#include <ncurses.h>

#include "ui/colors.hpp"

DoubleXslider::DoubleXslider() :/*{{{*/
	focused_slider { 0 }
{}/*}}}*/

void DoubleXslider::init(	const int32_t &_Ysize, const int32_t &_Xsize,/*{{{*/
							const int32_t &_Ypos, const int32_t &_Xpos ) noexcept
{
	Popup::init( _Ysize, _Xsize, _Ypos, _Xpos );
}/*}}}*/

void DoubleXslider::set_limits( const int16_t &_Lvalue, const int16_t &_Rvalue ) noexcept/*{{{*/
{
	if ( _Rvalue - _Lvalue + 1 != x_size ) {
		std::cerr <<
			"Extremos no coinciden con medida del ancho en DoubleXslider::set_extreme_values" <<
			std::endl;
		exit( EXIT_FAILURE );
	}

	min_value = _Lvalue;
	max_value = _Rvalue;
}/*}}}*/

void DoubleXslider::off() noexcept/*{{{*/
{
	is_active = false;
	lPos = 0;
	rPos = 0;

	wclear( area );
	wattron( area, COLOR_PAIR( dimmed_font.color ) );
	wattron( area, A_BOLD );
	set_font_reverse( true );
	mvwaddch( area, 0, 0, '>' );
	wrefresh( area );
}/*}}}*/

void DoubleXslider::on() noexcept/*{{{*/
{
	is_active = true;
	lPos = lValue - MIN_KEY + 1;
	rPos = rValue - MIN_KEY + 1;

	wclear( area );
	wattron( area, COLOR_PAIR( active_font.color ) );
	set_font_reverse( true );
	mvwaddch( area, 0, lPos, ' ' );
	active_font.width == "Bold" ? wattron( area, A_BOLD ) : wattroff( area, A_BOLD );
	set_font_reverse( false );
	for ( int16_t i = lPos + 1; i < rPos; ++i )
		mvwaddch( area, 0, i, '-' );
	set_font_reverse( true );
	mvwaddch( area, 0, rPos, ' ' );

	wrefresh( area );
}/*}}}*/

auto DoubleXslider::light() noexcept -> void/*{{{*/
{
	is_active = true;
	lPos = lValue - MIN_KEY + 1;
	rPos = rValue - MIN_KEY + 1;

	wclear(area);
	wattron(area, COLOR_PAIR(light_font.color));
	set_font_reverse(true);
	mvwaddch(area, 0, lPos, ' ');
	wattron(area, A_BOLD);
	set_font_reverse( false );
	for ( int16_t i = lPos + 1; i < rPos; ++i )
		mvwaddch( area, 0, i, '-' );
	set_font_reverse( true );
	mvwaddch( area, 0, rPos, ' ' );

	wrefresh( area );
}/*}}}*/

void DoubleXslider::set_values( const int16_t &_Lvalue, const int16_t &_Rvalue ) noexcept/*{{{*/
{
	// Como el control comienza con min_value, al recibir un valor absouluto obtiene la diferencia
	// para colocarlo visualmente en el lugar correcto.
	lValue = _Lvalue;
	rValue = _Rvalue;

}/*}}}*/

void DoubleXslider::set_cursor_at_left() noexcept/*{{{*/
{
	set_font_color( WHITE_DEFAULT );
	set_font_reverse( true );
	mvwaddch( area, 0, lPos, ' ' );
	curs_set( false );
	wrefresh( area );

	focused_slider = 0;
}/*}}}*/

void DoubleXslider::set_cursor_at_right() noexcept/*{{{*/
{
	set_font_color( WHITE_DEFAULT );
	set_font_reverse( true );
	mvwaddch( area, 0, rPos, ' ' );
	curs_set( false );
	wrefresh( area );

	focused_slider = 1;
}/*}}}*/

void DoubleXslider::leave_cursor() noexcept/*{{{*/
{
	if ( is_active ) {
		wattron( area, COLOR_PAIR( active_font.color ) );
		wattron( area, A_REVERSE );
		mvwaddch( area, 0, focused_slider == 0 ? lPos : rPos, ' ' );
	}
	else {
		wattron( area, COLOR_PAIR( dimmed_font.color ) );
		wattron( area, A_REVERSE );
		mvwaddch( area, 0, lPos, '>' );
	}

	wrefresh( area );
}/*}}}*/

void DoubleXslider::swap_cursor() noexcept/*{{{*/
{
	wattron( area, A_REVERSE ); // sí o sí
	if ( focused_slider == 0 ) {
		wattron( area, COLOR_PAIR( active_font.color ) );
		mvwaddch( area, 0, lPos, ' ' );
		wattron( area, COLOR_PAIR( cursor_font.color ) );
		mvwaddch( area, 0, rPos, ' ' );
		focused_slider = 1;
	}
	else {
		wattron( area, COLOR_PAIR( active_font.color ) );
		mvwaddch( area, 0, rPos, ' ' );
		wattron( area, COLOR_PAIR( cursor_font.color ) );
		mvwaddch( area, 0, lPos, ' ' );
		focused_slider = 0;
	}

	wrefresh( area );
}/*}}}*/

bool DoubleXslider::decrease_left_slider() noexcept/*{{{*/
{
	if ( lValue > MIN_KEY ) {
		wattron( area, COLOR_PAIR( active_font.color ) );
		active_font.width == "Bold" ? wattron( area, A_BOLD ) : wattroff( area, A_BOLD );
		if ( lValue == rValue ) { // Si están a la par, en vez de poner raya pondrá fader
			wattron( area, A_REVERSE );
			mvwaddch( area, 0, lPos, ' ' );
		}
		else {
			wattroff( area, A_REVERSE );
			mvwaddch( area, 0, lPos, '-' );
		}
		--lValue;
		--lPos;
		wattron( area, COLOR_PAIR( cursor_font.color ) );
		wattron( area, A_REVERSE );
		mvwaddch( area, 0, lPos, ' ' );

		wrefresh( area );

		return true;
	}
	else
		return false;
}/*}}}*/

bool DoubleXslider::decrease_right_slider() noexcept/*{{{*/
{
	if ( rValue > lValue ) {
		wattroff( area, A_REVERSE );
		mvwaddch( area, 0, rPos, ' ' );
		--rValue;
		--rPos;
		wattron( area, COLOR_PAIR( cursor_font.color ) );
		wattron( area, A_REVERSE );
		mvwaddch( area, 0, rPos, ' ' );

		wrefresh( area );

		return true;
	}
	else
		return false;
}/*}}}*/

bool DoubleXslider::increase_left_slider() noexcept/*{{{*/
{
	if ( lValue < rValue ) {
		wattroff( area, A_REVERSE );
		mvwaddch( area, 0, lPos, ' ' );
		++lValue;
		++lPos;
		wattron( area, COLOR_PAIR( cursor_font.color ) );
		wattron( area, A_REVERSE );
		mvwaddch( area, 0, lPos, ' ' );

		wrefresh( area );
		return true;
	}
	else
		return false;
}/*}}}*/

bool DoubleXslider::increase_right_slider() noexcept/*{{{*/
{
	if ( rValue < MAX_KEY ) {
		wattron( area, COLOR_PAIR( active_font.color ) );
		active_font.width == "Bold" ? wattron( area, A_BOLD ) : wattroff( area, A_BOLD );
		if ( lValue == rValue ) { // Lo mismo aquí
			wattron( area, A_REVERSE );
			mvwaddch( area, 0, rPos, ' ' );
		}
		else {
			wattroff( area, A_REVERSE );
			mvwaddch( area, 0, rPos, '-' );
		}
		++rValue;
		++rPos;
		wattron( area, COLOR_PAIR( cursor_font.color ) );
		wattron( area, A_REVERSE );
		mvwaddch( area, 0, rPos, ' ' );

		wrefresh( area );
		return true;
	}
	else
		return false;
}/*}}}*/

void DoubleXslider::clean() noexcept/*{{{*/
{
	wclear( area );
	update();
}/*}}}*/
