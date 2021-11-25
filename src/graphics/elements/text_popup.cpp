#include "graphics/elements/text_popup.hpp"
#include <ncurses.h>
#include <string>
#include <string.h>

void TextPopup::set_text( const char *_Texto ) noexcept
{
	wclear( area );
	waddstr( area, _Texto );
	wrefresh( area );
}	

void TextPopup::set_text( const std::string &_Texto ) noexcept
{
	wclear( area );
	waddstr( area, _Texto.c_str() );
	wrefresh( area );
}
