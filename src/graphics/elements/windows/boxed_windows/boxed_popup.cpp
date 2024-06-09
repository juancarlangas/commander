#include "graphics/elements/windows/boxed_windows/boxed_popup.hpp"


void BoxedPopup::init(	const int32_t _Ysize, const int32_t _Xsize,
						const int32_t _Ypos, const int32_t _Xpos,
						const int32_t _Frame_Width ) noexcept
{
	box = newwin( _Ysize, _Xsize, _Ypos, _Xpos );
	Window::init( _Ysize - _Frame_Width * 2, _Xsize - _Frame_Width * 2,
					_Ypos + _Frame_Width, _Xpos + _Frame_Width );
	BoxedPopup::box_panel = new_panel( BoxedWindow::box );
	BoxedPopup::area_panel = new_panel( BoxedWindow::area );
}

void BoxedPopup::show() noexcept
{
	show_panel( BoxedPopup::box_panel );
	show_panel( BoxedPopup::area_panel );
}

void BoxedPopup::hide() noexcept
{
	hide_panel( BoxedPopup::area_panel );
	hide_panel( BoxedPopup::box_panel );
}
