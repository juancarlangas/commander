#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <ncurses.h>
#include <panel.h>
#include <string_view>

class Popup {
	public:
		void init( const int32_t, const int32_t, const int32_t, const int32_t ) noexcept;
		void set_color( const int32_t &color ) noexcept;
		void set_font_width( const std::string_view );
		void set_borders( 	const int32_t &, const int32_t &,
							const int32_t &, const int32_t &,
							const int32_t &, const int32_t &,
							const int32_t &, const int32_t &) noexcept;
		void draw() noexcept;
		void update() noexcept;
		void show() noexcept;
		void hide() noexcept;
	private:
		WINDOW *ventana; // sencillito porque es una perrazera, cada ventana requiere su panel !!!!
		PANEL *panel;
		int32_t y_size, x_size, y_pos, x_pos;
};

#endif
