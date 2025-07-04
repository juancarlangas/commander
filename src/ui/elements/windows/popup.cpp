#include "popup.hpp"

void Popup::init(const int32_t _Ysize, const int32_t _Xsize, const int32_t _Ypos, const int32_t _Xpos) noexcept
{
	Window::init( _Ysize, _Xsize, _Ypos, _Xpos );
	Popup::area_panel = new_panel(area);
}

void Popup::show() noexcept/*{{{*/
{
	show_panel(Popup::area_panel);
}/*}}}*/

void Popup::hide() noexcept/*{{{*/
{
	hide_panel( Popup::area_panel );
}/*}}}*/
