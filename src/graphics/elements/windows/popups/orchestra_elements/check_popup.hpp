#ifndef CHECK_POPUP_HPP
#define CHECK_POPUP_HPP

#include "graphics/elements/windows/popups/orchestra_element.hpp"

class CheckPopup : public OrchestraElement {
	public:
		void init( const int32_t &_Ypos, const int32_t &_Xpos, const struct Font &,
				const struct Font &, const struct Font & ) noexcept;
		void init( const int32_t &_Ypos, const int32_t &_Xpos, const struct Font &,
				const struct Font &, const struct Font &, const char &_Caption ) noexcept;
		void on() noexcept;
		void off() noexcept;
		void set_cursor() noexcept;
		void leave_cursor() noexcept;
	private:
		bool is_active;
		char caption;
};
#endif
