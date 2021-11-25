#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <bits/stdint-intn.h>
#include <panel.h>
#include <string_view>
#include <ncurses.h>

class Window {
	public:
		void init( const int32_t, const int32_t, const int32_t, const int32_t ) noexcept;
		void set_box_color( const int32_t &color ) noexcept;
		void set_box_width( const std::string_view ) noexcept;
		void define_box(	const int32_t &_Top, const int32_t &_Right,
							const int32_t &_Bottom, const int32_t &_Left,
							const int32_t &_TopLeft, const int32_t &_TopRight,
							const int32_t &_BottomRight, const int32_t &_BottomLeft ) noexcept;
		void set_font_color( const int32_t &color ) noexcept;
		void set_font_width( const std::string_view ) noexcept;
		void draw() noexcept;
		void update() noexcept;
	protected:
		WINDOW *box, *area;
		int32_t y_size, x_size, y_pos, x_pos;
};

class Popup : public Window {
	public:
		void init( const int32_t, const int32_t, const int32_t, const int32_t ) noexcept;
		void show() noexcept;
		void hide() noexcept;
	protected:
		PANEL *box_panel, *area_panel;
};

#endif
