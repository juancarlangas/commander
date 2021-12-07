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

void Popup::show() noexcept
{
	show_panel( area_panel );
}

void Popup::hide() noexcept
{
	hide_panel( area_panel );
}
