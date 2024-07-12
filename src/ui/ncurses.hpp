#ifndef NCURSES_HPP
#define NCURSES_HPP

#define Y_PIXELS 32
#define X_PIXELS 127

#include <cstdint>
#include <ncurses.h>
#include <panel.h>

#include "ui/orchestra.hpp"

extern int screen_width, screen_height;
extern short displayShowResults, playlistShowResults;	

extern WINDOW 	*searchBox,		*searchWindow,
				*lcdWindow,
				*zoomBox,		*zoomWindow,
				*displayBox,	*displayWindow,
				*playlistBox,	*playlistWindow,
				*MIDI_state_window;

extern WINDOW 	*ventana[2];
extern PANEL 	*panel[2];

extern Orchestra orquestacion;

void ncurses_start_sequence() noexcept;
void ncurses_end_sequence() noexcept;
auto set_windows(void) noexcept -> void;
void draw_windows(void);
void tint_lcd(const short int mode);
void update_popups() noexcept;
auto get_y_pixels() noexcept -> std::int16_t;

class Screen {
	public:
		Screen();
		~Screen();
		void on() noexcept;
	private:
		friend class ventana;
};

#endif
