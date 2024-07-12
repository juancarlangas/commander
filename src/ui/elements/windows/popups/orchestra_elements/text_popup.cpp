#include "text_popup.hpp"
#include <ncurses.h>
#include <string>

TextPopup::TextPopup()/*{{{*/
{}/*}}}*/

void TextPopup::set_text( const char *&_Texto ) noexcept/*{{{*/
{
	set_caption( _Texto );
	update_caption();
}/*}}}*/

void TextPopup::set_text( const std::string &_Texto ) noexcept/*{{{*/
{
	set_caption( _Texto );
	update_caption();
}/*}}}*/

void TextPopup::set_text( const char &_Caracter ) noexcept/*{{{*/
{
	const std::string &cadena_temporal { _Caracter };
	set_caption( cadena_temporal );
	update_caption();
}/*}}}*/

void TextPopup::set_value( const int32_t &_Integer ) noexcept/*{{{*/
{
	const std::string &cadena_temporal = std::to_string( _Integer );
	set_caption( cadena_temporal );
	update_caption();
}/*}}}*/

void TextPopup::set_caption( const std::string_view _Caption ) noexcept/*{{{*/
{
	caption = _Caption;
}/*}}}*/

void TextPopup::update_caption() noexcept/*{{{*/
{
	wclear( area );
	waddstr( area, caption.c_str() );
	wrefresh( area );
}/*}}}*/
