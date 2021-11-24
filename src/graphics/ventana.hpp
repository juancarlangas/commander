#ifndef VENTANA_HPP
#define VENTANA_HPP

#include <bits/stdint-intn.h>
#include <string_view>
#include <ncurses.h>

class Ventana {
	public:
		void init( const int32_t, const int32_t, const int32_t, const int32_t ) noexcept;
		void set_font_color( const int32_t &color ) noexcept;
		void set_font_width( const std::string_view ) noexcept;
		void set_border_symbols(	const int32_t &, const int32_t &,
									const int32_t &, const int32_t &,
									const int32_t &, const int32_t &,
									const int32_t &, const int32_t & ) noexcept;
		void draw() noexcept;
		void update() noexcept;
	protected:
		WINDOW *ventana;
		int32_t y_size, x_size, y_pos, x_pos;
};

#endif
