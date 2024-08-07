#include "check_popup.hpp"
#include "ui/colors.hpp"
#include <ncurses.h>

void CheckPopup::init( const int32_t &_Ypos, const int32_t &_Xpos, const struct Font &_ActiveFont,/*{{{*/
		const struct Font &_CursorFont, const struct Font &_DimmedFont, const Font& _LightFont) noexcept
{
	OrchestraElement::init( 1, 1, _Ypos, _Xpos, _ActiveFont, _CursorFont, _DimmedFont, _LightFont );
	wattron( area, A_BOLD );
}/*}}}*/

void CheckPopup::init( const int32_t &_Ypos, const int32_t &_Xpos, const struct Font &_ActiveFont,/*{{{*/
	const struct Font &_CursorFont, const struct Font &_DimmedFont, const Font& _LightFont, const char &_Caption ) noexcept
{
	caption = _Caption;
	init(_Ypos, _Xpos, _ActiveFont, _CursorFont, _DimmedFont, _LightFont);
}/*}}}*/

void CheckPopup::on() noexcept/*{{{*/
{
	is_active = true;
	wclear(area);
	wattron(area, COLOR_PAIR(active_font.color));
	active_font.width == "Bold" ? wattron(area, A_BOLD) : wattroff(area, A_BOLD);
	wattroff(area, A_REVERSE);
	mvwaddch(area, 0, 0, caption);
	wrefresh(area);
}/*}}}*/

auto CheckPopup::light() noexcept -> void/*{{{*/
{
	is_active = true;
	wclear(area);
	wattron(area, COLOR_PAIR(light_font.color));
	wattron(area, A_BOLD);
	wattroff(area, A_REVERSE);
	mvwaddch(area, 0, 0, caption);
	wrefresh(area);
}/*}}}*/

void CheckPopup::off() noexcept/*{{{*/
{
	is_active = false;
	wclear( area );
	wattron( area, COLOR_PAIR(dimmed_font.color));
	wattroff( area, A_REVERSE );
	mvwaddch( area, 0, 0, caption );
	wrefresh( area );
}/*}}}*/

void CheckPopup::set_cursor() noexcept/*{{{*/
{
	set_font_color( WHITE_DEFAULT );
	set_font_reverse( true );
	mvwaddch( area, 0, 0, caption );
	curs_set( false );
	wrefresh( area );
}/*}}}*/

void CheckPopup::leave_cursor() noexcept/*{{{*/
{
	if ( is_active ) {
		wattron( area, COLOR_PAIR( active_font.color ) );
		active_font.width == "Bold" ? wattron( area, A_BOLD ) : wattroff( area, A_BOLD );
		wattroff( area, A_REVERSE );
		mvwaddch( area, 0, 0, caption );
	}
	else {
		wattron( area, COLOR_PAIR( dimmed_font.color ) );
		wattroff( area, A_REVERSE );
		mvwaddch( area, 0, 0, caption );
	}
	wrefresh( area );
}/*}}}*/
