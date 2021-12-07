#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <bits/stdint-intn.h>
#include <panel.h>
#include <string_view>
#include <ncurses.h>
#include <string>
#include <array>

struct Font {
	int32_t color;
	std::string style;
};

class Window {
	public:
		void init( const int32_t, const int32_t, const int32_t, const int32_t ) noexcept;
		void init( const int32_t, const int32_t, const int32_t, const int32_t,
				const struct Font &, const struct Font &, const struct Font & ) noexcept;
		void set_font_color( const int32_t &color ) noexcept;
		void set_font_width( const std::string_view ) noexcept;
		void set_font_reverse( const bool & ) noexcept;
		void set_font_blinking( const bool & ) noexcept;
		void update() noexcept;
	protected:
		WINDOW *area;
		int32_t y_size, x_size, y_pos, x_pos;
		struct Font inherit_font;
		struct Font cursor_font;
		struct Font dimmed_font;
};


#endif
