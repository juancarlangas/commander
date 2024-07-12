#include "field_popup.hpp"
#include <ncurses.h>

void FieldPopup::on() noexcept/*{{{*/
{
	is_active = true;

	wattron( area, COLOR_PAIR( active_font.color ) );
	active_font.width == "Bold" ? wattron( area, A_BOLD ) : wattroff( area, A_BOLD );
}/*}}}*/

void FieldPopup::off() noexcept/*{{{*/
{
	is_active = false;

	wattron( area, COLOR_PAIR( dimmed_font.color ) );
	dimmed_font.width == "Bold" ? wattron( area, A_BOLD ) : wattroff( area, A_BOLD );
}/*}}}*/

auto FieldPopup::light() noexcept -> void/*{{{*/
{
	is_active = true;

	wattron(area, COLOR_PAIR(light_font.color));
	wattron(area, A_BOLD);
}/*}}}*/

void FieldPopup::set_cursor() noexcept/*{{{*/
{
	// Ponemos el color del cursor
	wattron( area, COLOR_PAIR( cursor_font.color ) );
	cursor_font.width == "Bold" ? wattron( area, A_BOLD ) : wattroff( area, A_BOLD );

	// Colocamos el cursor
	waddstr( area, "" );
	curs_set( true );

	// reestablecemos el color
	if ( is_active ) {
		set_font_color( active_font.color );
		set_font_width( active_font.width );
	}
	else {
		set_font_color( dimmed_font.color );
		set_font_width( dimmed_font.width );
	}

	wrefresh( area );
}/*}}}*/
