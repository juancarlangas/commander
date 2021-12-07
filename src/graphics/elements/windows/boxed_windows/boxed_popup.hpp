#ifndef FRAMED_POPUP_HPP
#define FRAMED_POPUP_HPP

#include "graphics/elements/windows/boxed_window.hpp"

class BoxedPopup : public BoxedWindow {
	public:
		void init( const int32_t, const int32_t, const int32_t, const int32_t,
					const int32_t ) noexcept;
		void show() noexcept;
		void hide() noexcept;
	protected:
		PANEL *box_panel, *area_panel;
};

#endif
