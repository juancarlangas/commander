#include "field_popup.hpp"
#include <ncurses.h>

void FieldPopup::on() noexcept
{
	wattron( area, COLOR_PAIR( inherit_font.color ) );
	inherit_font.style == "Bold" ? wattron( area, A_BOLD ) : wattroff( area, A_BOLD );
}

void FieldPopup::off() noexcept
{
	wattron( area, COLOR_PAIR( dimmed_font.color ) );
	dimmed_font.style == "Bold" ? wattron( area, A_BOLD ) : wattroff( area, A_BOLD );
}

void FieldPopup::set_cursor() noexcept
{
	wattron( area, COLOR_PAIR( cursor_font.color ) );
	cursor_font.style == "Bold" ? wattron( area, A_BOLD ) : wattroff( area, A_BOLD );

	waddstr( area, "" );
	curs_set( true );
	wrefresh( area );
}
