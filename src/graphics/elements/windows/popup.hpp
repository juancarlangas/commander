#ifndef POPUP_HPP
#define POPUP_HPP

#include "graphics/elements/window.hpp"

class Popup : public Window {
	public:
		void init( const int32_t, const int32_t, const int32_t, const int32_t ) noexcept;
		void show() noexcept;
		void hide() noexcept;
	protected:
		PANEL *area_panel;
};


#endif
