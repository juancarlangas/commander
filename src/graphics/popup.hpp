#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <ncurses.h>
#include <panel.h>
#include <string_view>
#include "graphics/ventana.hpp"

class Popup : public Ventana {
	public:
		void init( const int32_t, const int32_t, const int32_t, const int32_t ) noexcept;
		void show() noexcept;
		void hide() noexcept;
	protected:
		PANEL *panel;
};

#endif
