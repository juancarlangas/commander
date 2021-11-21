#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <ncurses.h>

class Ventana {
	public:
		Ventana();
		~Ventana();
		void init( const int32_t, const int32_t, const int32_t, const int32_t ) noexcept;
		void set_box_color( const int32_t &color ) noexcept;
	private:
		WINDOW *box, *area;
};

#endif
