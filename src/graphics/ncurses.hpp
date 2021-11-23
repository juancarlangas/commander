#ifndef NCURSES_HPP
#define NCURSES_HPP

#include "graphics/popup.hpp"

short int init_ncurses(void);
void draw_windows(void);
void tint_lcd(const short int mode);

class Screen {
	public:
		Screen();
		~Screen();
		void on() noexcept;
	private:
		friend class ventana;
};

#endif
