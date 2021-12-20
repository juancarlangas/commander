#ifndef TEXT_POPUP_HPP
#define TEXT_POPUP_HPP

#include "graphics/elements/windows/popups/orchestra_element.hpp"
#include <string_view>

class TextPopup : public OrchestraElement {
	public:
		TextPopup();
		void set_text( const char *& ) noexcept;
		void set_text( const std::string & ) noexcept;
		void set_text( const char & ) noexcept;
		void set_value( const int32_t & ) noexcept;
	protected:
		void set_caption( const std::string_view ) noexcept;
		void update_caption() noexcept;
		std::string caption;
};

#endif
