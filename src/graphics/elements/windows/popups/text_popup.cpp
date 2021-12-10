#include "text_popup.hpp"
#include <ncurses.h>
#include <string>
#include <string.h>

void TextPopup::set_text( const char *_Texto ) noexcept
{
	wclear( area );
	waddstr( area, _Texto );
	wrefresh( area );
}	

void TextPopup::set_text( const std::string &_Texto ) noexcept/*{{{*/
{
	wclear( area );
	waddstr( area, _Texto.c_str() );
	wrefresh( area );
}/*}}}*/

void TextPopup::set_text( const char &_Caracter ) noexcept
{
	wclear( area );
	waddch( area, _Caracter );
	wrefresh( area );
}

void TextPopup::set_value( const int32_t &_Integer ) noexcept
{
	wclear( area );
	wprintw( area, "%d", _Integer );
	wrefresh( area );
}
