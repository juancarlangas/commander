#include "graphics/elements/windows/popups/orchestra_element.hpp"

OrchestraElement::OrchestraElement() :/*{{{*/
	is_active { false }
{}/*}}}*/

void OrchestraElement::init( const int32_t &_Ysize, const int32_t &_Xsize,/*{{{*/
		const int32_t &_Ypos, const int32_t &_Xpos,
		const struct Font &_ActiveFont, const struct Font &_DimmedFont,
		const struct Font &_CursorFont ) noexcept
{
	Popup::init( _Ysize, _Xsize, _Ypos, _Xpos );

	active_font = _ActiveFont;
	dimmed_font = _DimmedFont;
	cursor_font = _CursorFont;
}/*}}}*/

