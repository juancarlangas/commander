#ifndef FIELD_POPUP_HPP
#define FIELD_POPUP_HPP

#include "graphics/elements/windows/popups/text_popup.hpp"

class FieldPopup : public TextPopup {
	public:
		void on() noexcept;
		void off() noexcept;
		void set_cursor() noexcept;
};

#endif
