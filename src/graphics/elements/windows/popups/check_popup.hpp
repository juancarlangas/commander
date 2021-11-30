#ifndef CHECK_POPUP_HPP
#define CHECK_POPUP_HPP

#include "graphics/elements/windows/popup.hpp"

class CheckPopup : public Popup {
	public:
		CheckPopup();
		void init( const int32_t &_Ypos, const int32_t &_Xpos, const struct Font &,
				const struct Font &, const struct Font & ) noexcept;
		void on() noexcept;
		void off() noexcept;
		void set_cursor() noexcept;
		void leave_cursor() noexcept;
	private:
		bool is_active;
};
#endif
