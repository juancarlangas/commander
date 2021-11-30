#include "check_popup.hpp"
#include "graphics/colors.hpp"
#include <ncurses.h>

CheckPopup::CheckPopup() :
	is_active { false }
{}

void CheckPopup::init( const int32_t &_Ypos, const int32_t &_Xpos, const struct Font &_InheritFont,
		const struct Font &_CursorFont, const struct Font &_DimmedFont ) noexcept
{
	Popup::init( 1, 1, _Ypos, _Xpos, _InheritFont, _CursorFont, _DimmedFont );
	wattron( area, A_BOLD );
}

void CheckPopup::on() noexcept
{
	is_active = true;
	wclear( area );
	wattron( area, COLOR_PAIR( inherit_font.color ) );
	wattron( area, A_REVERSE );
	mvwaddch( area, 0, 0, ' ' );
	wrefresh( area );
}

void CheckPopup::off() noexcept
{
	is_active = false;
	wclear( area );
	wattron( area, COLOR_PAIR( dimmed_font.color ) );
	wattroff( area, A_REVERSE );
	mvwaddch( area, 0, 0, '-' );
	wrefresh( area );
}

void CheckPopup::set_cursor() noexcept
{
	set_font_color( WHITE_DEFAULT );
	set_font_reverse( true );
	mvwaddch( area, 0, 0, ' ' );
	curs_set( false );
	wrefresh( area );
}

void CheckPopup::leave_cursor() noexcept
{
	if ( is_active ) {
		wattron( area, COLOR_PAIR( inherit_font.color ) );
		wattron( area, A_REVERSE );
		mvwaddch( area, 0, 0, ' ' );
	}
	else {
		wattron( area, COLOR_PAIR( dimmed_font.color ) );
		wattroff( area, A_REVERSE );
		mvwaddch( area, 0, 0, '-' );
	}
	wrefresh( area );
}
