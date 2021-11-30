#ifndef TEXT_POPUP_HPP
#define TEXT_POPUP_HPP

#include "graphics/elements/windows/popup.hpp"

class TextPopup : public Popup {
	public:
		void set_text( const char * ) noexcept;
		void set_text( const std::string & ) noexcept;
		void set_text( const char & ) noexcept;
		void set_value( const int32_t & ) noexcept;
	private:
		std::string texto;
};

#endif
