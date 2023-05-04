#ifndef NCURSES_HPP
#define NCURSES_HPP

#include <cstdint>

#include "graphics/elements/window.hpp"
#include "graphics/elements/windows/popups/orchestra_elements/text_popups/field_popup.hpp"

#define Y_PIXELS 32
#define X_PIXELS 127

short int init_ncurses(void);
void init_screen() noexcept;
void end_screen() noexcept;
void draw_windows(void);
void tint_lcd(const short int mode);
void update_popups() noexcept;

class Screen {
	public:
		Screen();
		~Screen();
		void on() noexcept;
	private:
		friend class ventana;
};

#endif
