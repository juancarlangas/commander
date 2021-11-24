#ifndef TEXT_POPUP_HPP
#define TEXT_POPUP_HPP

#include "graphics/popup.hpp"

class TextPopup : public Popup {
	public:
		void set_text( const std::string & ) noexcept;
};

#endif
