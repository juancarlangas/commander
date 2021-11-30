#include "popup.hpp"


void Popup::init(	const int32_t _Ysize, const int32_t _Xsize,
					const int32_t _Ypos, const int32_t _Xpos ) noexcept
{
	Window::init( _Ysize, _Xsize, _Ypos, _Xpos );
	area_panel = new_panel( area );
}

void Popup::init( const int32_t _Ysize, const int32_t _Xsize,
		const int32_t _Ypos, const int32_t _Xpos,
		const struct Font &_NativeFont, const struct Font &_CursorFont,
		const struct Font &_DimmedFont ) noexcept
{
	Window::init( _Ysize, _Xsize, _Ypos, _Xpos, _NativeFont, _CursorFont, _DimmedFont );
	area_panel = new_panel( area );
}

void BoxedPopup::init(	const int32_t _Ysize, const int32_t _Xsize,
						const int32_t _Ypos, const int32_t _Xpos,
						const int32_t _Frame_Width ) noexcept
{
	box = newwin( _Ysize, _Xsize, _Ypos, _Xpos );
	Window::init( _Ysize - _Frame_Width * 2, _Xsize - _Frame_Width * 2,
					_Ypos + _Frame_Width, _Xpos + _Frame_Width );
	box_panel = new_panel( box );
	area_panel = new_panel( area );
}

void Popup::show() noexcept
{
	show_panel( area_panel );
}

void Popup::hide() noexcept
{
	hide_panel( area_panel );
}

void BoxedPopup::show() noexcept
{
	show_panel( box_panel );
	show_panel( area_panel );
}

void BoxedPopup::hide() noexcept
{
	hide_panel( area_panel );
	hide_panel( box_panel );
}
