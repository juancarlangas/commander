#ifndef NCURSES_HPP
#define NCURSES_HPP

#define Y_PIXELS 32
#define X_PIXELS 127

#include <cstdint>
#include <ncurses.h>
#include <panel.h>

#include "ui/Orchestra.hpp"

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

void tint_lcd(const short int mode);
void update_popups() noexcept;
auto get_y_pixels() noexcept -> std::int16_t;

// Members are destroyed in reverse order: ncurses must outlive its windows.
class NcursesSession {
	public:
		NcursesSession() noexcept;
		~NcursesSession() noexcept;
		NcursesSession(const NcursesSession&) = delete;
		NcursesSession& operator=(const NcursesSession&) = delete;
};

class Screen {
	public:
		Screen() noexcept;
		~Screen() noexcept;
		Screen(const Screen&) = delete;
		Screen& operator=(const Screen&) = delete;
		void resize() noexcept;
		void draw();
		Orchestra& orchestra() noexcept { return orchestra_; }
	private:
		NcursesSession session_;
		Orchestra orchestra_;
};

#endif
