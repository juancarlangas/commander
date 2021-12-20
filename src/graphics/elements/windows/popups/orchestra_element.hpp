#ifndef ORCHESTRA_ELEMENT_HPP
#define ORCHESTRA_ELEMENT_HPP

#include "graphics/elements/windows/popup.hpp"
#include <bits/stdint-intn.h>

class OrchestraElement : public Popup {
	public:
		OrchestraElement();
		void init( const int32_t &_Ysize, const int32_t &_Xsize,
				const int32_t &_Ypos, const int32_t &_Xpos,
				const struct Font &_ActiveFont, const struct Font &_DimmedFont,
				const struct Font &_CursorFont ) noexcept;
	protected:
		bool is_active;
		struct Font active_font;
		struct Font dimmed_font;
		struct Font cursor_font;
};

#endif
