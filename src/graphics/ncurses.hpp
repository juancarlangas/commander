#ifndef NCURSES_HPP
#define NCURSES_HPP

#include "graphics/elements/window.hpp"
#include "graphics/elements/windows/popups/text_popups/field_popup.hpp"

#define Y_PIXELS 32
#define X_PIXELS 127

short int init_ncurses(void);
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
