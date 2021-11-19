#ifndef ORCHESTRA_HPP
#define ORCHESTRA_HPP

#include <array>
#include <ncurses.h>
#include <panel.h>

class Orchestra {
	private:
		PANEL *panel;
		WINDOW *box, *area;
		WINDOW *label;
		std::array<WINDOW, 4> caption;
		std::array<std::array<WINDOW, 4>, 8> field;
};

#endif
