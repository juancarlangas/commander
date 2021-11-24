#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <ncurses.h>
#include <panel.h>
#include <string_view>

class Popup {
	public:
		void init( const int32_t, const int32_t, const int32_t, const int32_t ) noexcept;
		void set_border_color( const int32_t &color ) noexcept;
		void set_border_font_width( const std::string_view );
		void set_border_symbols(	const int32_t &, const int32_t &,
									const int32_t &, const int32_t &,
									const int32_t &, const int32_t &,
									const int32_t &, const int32_t & ) noexcept;
		void draw() noexcept;
		void update() noexcept;
		void show() noexcept;
		void hide() noexcept;
	protected:
		WINDOW *win_box, *win_area;
		PANEL *panel_box, *panel_area;
		int32_t y_size, x_size, y_pos, x_pos;
};

#endif
