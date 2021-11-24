#include "graphics/popups/text_popup.hpp"
#include <ncurses.h>
#include <string>
#include <string.h>

void TextPopup::set_text( const std::string &_Texto ) noexcept
{
	wclear( ventana );
	wprintw( ventana, _Texto.c_str() );
	wrefresh( ventana );
}	
