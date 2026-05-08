#include "common/common.hpp"
#include "ui/colors.hpp"
#include "ui/ncurses.hpp"

#include <algorithm>
#include <stdlib.h>

int screen_width, screen_height;
short displayShowResults, playlistShowResults;

WINDOW 	*searchBox,     *searchWindow,
		*lcdWindow,
		*zoomBox,       *zoomWindow,
		*displayBox,    *displayWindow,
		*playlistBox,   *playlistWindow,
		*MIDI_state_window;

WINDOW 	*ventana[2];
PANEL 	*panel[2];

Orchestra orquestacion;

namespace {
int safe_dimension(const int value, const int minimum = 1) noexcept
{
	return std::max(value, minimum);
}

void delete_window(WINDOW *&window) noexcept
{
	if (window != nullptr) {
		delwin(window);
		window = nullptr;
	}
}

void delete_panel_window(PANEL *&panel_ref, WINDOW *&window) noexcept
{
	if (panel_ref != nullptr) {
		del_panel(panel_ref);
		panel_ref = nullptr;
	}
	delete_window(window);
}

void create_windows() noexcept
{
	getmaxyx(stdscr, screen_height, screen_width);

	const int layout_height = std::max(screen_height, 1);
	const int layout_width = std::max(screen_width, 1);

	const int content_top = std::min(9, std::max(1, layout_height / 3));
	const int top = std::min(3, std::max(0, content_top - 1));
	const int search_height = std::min(5, std::max(1, layout_height - content_top - 1));
	const int search_y = std::max(content_top + 1, layout_height - search_height);

	const int left_width = safe_dimension(layout_width * 142 / 200);
	const int right_x = std::min(layout_width - 1, layout_width * 141 / 200);
	const int right_width = safe_dimension(layout_width - right_x);

	const int zoom_height = std::min(7, std::max(3, content_top));
	const int list_height = safe_dimension(search_y - content_top);
	const int search_width = safe_dimension(layout_width * 140 / 200);
	const int lcd_height = safe_dimension(layout_height * 30 / 200 - 2, 3);
	const int lcd_width = safe_dimension(layout_width * 98 / 200 - 2);
	const int lcd_y = layout_height * 20 / 200 + 1;
	const int lcd_x = layout_width * 98 / 200 + 1;

	searchBox = newwin(search_height, search_width, search_y, 0);
	searchWindow = newwin(safe_dimension(search_height - 2), safe_dimension(search_width - 2), search_y + 1, 1);

	lcdWindow = newwin(lcd_height, lcd_width, lcd_y, lcd_x);

	playlistBox = newwin(list_height, right_width, content_top, right_x);
	playlistWindow = newwin(safe_dimension(list_height - 3), safe_dimension(right_width - 2), content_top + 2, right_x + 1);

	displayBox = newwin(list_height, left_width, content_top, 0);
	displayWindow = newwin(safe_dimension(list_height - 3), safe_dimension(left_width - 2), content_top + 2, 1);

	zoomBox = newwin(zoom_height, safe_dimension(layout_width * 98 / 200), top, 0);
	zoomWindow = newwin(safe_dimension(zoom_height - 2), safe_dimension(layout_width * 98 / 200 - 2), top + 1, 1);

	MIDI_state_window = newwin(1, 4, std::min(2, layout_height - 1), std::max(0, std::min(layout_width - 4, layout_width * 180 / 200)));

	ventana[DIALOG_WINDOW] = newwin(3, safe_dimension(layout_width * 40 / 100), layout_height * 40 / 100, layout_width * 30 / 100);
	ventana[INPUT_BOX] = newwin(1, safe_dimension(layout_width * 40 / 100 - 2), layout_height * 40 / 100 + 1, layout_width * 30 / 100 + 1);

	panel[DIALOG_WINDOW] = new_panel(ventana[DIALOG_WINDOW]);
	panel[INPUT_BOX] = new_panel(ventana[INPUT_BOX]);

	hide_panel(panel[DIALOG_WINDOW]);
	hide_panel(panel[INPUT_BOX]);

	displayShowResults = safe_dimension(list_height - 4, 0);
	playlistShowResults = safe_dimension(list_height - 4, 0);

	orquestacion.init(layout_height * 180 / 200, layout_width * 180 / 200, layout_height * 20 / 200, layout_width * 10 / 200);
}

void destroy_windows() noexcept
{
	delete_panel_window(panel[INPUT_BOX], ventana[INPUT_BOX]);
	delete_panel_window(panel[DIALOG_WINDOW], ventana[DIALOG_WINDOW]);
	delete_window(MIDI_state_window);
	delete_window(zoomWindow);
	delete_window(zoomBox);
	delete_window(displayWindow);
	delete_window(displayBox);
	delete_window(playlistWindow);
	delete_window(playlistBox);
	delete_window(lcdWindow);
	delete_window(searchWindow);
	delete_window(searchBox);
}
} // namespace

void ncurses_start_sequence() noexcept/*{{{*/
{
	initscr();
	raw();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(FALSE);
	set_escdelay(3);
}/*}}}*/

void ncurses_end_sequence() noexcept/*{{{*/ {
	set_escdelay(0);
	curs_set(TRUE);
	keypad(stdscr, FALSE);
	echo();
	noraw();
	endwin();
}/*}}}*/

auto set_windows(void) noexcept -> void/*{{{*/
{
	ncurses_start_sequence();

	if (has_colors() == TRUE) {
		start_color();
		use_default_colors();

		init_pair(GRAY_DEFAULT, 	COLOR_BLACK, 	-1);
		init_pair(WHITE_DEFAULT, 	COLOR_WHITE, 	-1);
		init_pair(BLUE_DEFAULT,		COLOR_BLUE, 	-1);
		init_pair(GREEN_DEFAULT, 	COLOR_GREEN, 	-1);
		init_pair(YELLOW_DEFAULT,	COLOR_YELLOW,	-1);
		init_pair(MAGENTA_DEFAULT,	COLOR_MAGENTA,	-1);
		init_pair(CYAN_DEFAULT,		COLOR_CYAN,		-1);
		init_pair(RED_DEFAULT,		COLOR_RED,		-1);
		init_pair(WHITE_BLACK,		COLOR_WHITE, 	COLOR_BLACK);
		init_pair(BLACK_GRAY,		COLOR_BLACK,	COLOR_WHITE);

	}

	create_windows();
	refresh();
}/*}}}*/

auto resize_windows(void) noexcept -> void/*{{{*/
{
	endwin();
	refresh();
	clear();
	erase();
	destroy_windows();
	create_windows();
	clearok(curscr, TRUE);
	refresh();
}/*}}}*/
	
void draw_windows(void)/*{{{*/
{
	/* playlistBox */
		werase(playlistBox);
		wattron(playlistBox, COLOR_PAIR(GRAY_DEFAULT));
		wattron(playlistBox, A_BOLD);
		wborder(playlistBox, 0, 0, 0, 0, ACS_TTEE, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
		mvwprintw(playlistBox, 1, std::min(12, std::max(1, screen_width / 20)), " PlayList ");
		wrefresh(playlistBox);

	/* displayBox */
		werase(displayBox);
		wattron(displayBox, COLOR_PAIR(GRAY_DEFAULT));
		wattron(displayBox, A_BOLD);
		wborder(displayBox, 0, 0, 0, 0, ACS_LTEE, ACS_TTEE, ACS_LLCORNER, ACS_RTEE);
		mvwprintw(displayBox, 1, 36, " SONG ");
		mvwprintw(displayBox, 1, 41, " ARTIST ");
		mvwprintw(displayBox, 1, 72, " GENRE ");
		mvwprintw(displayBox, 1, 96, " KEYWORDS ");
		wrefresh(displayBox);

	/* zoomBox*/
		werase(zoomBox);
		wattron(zoomBox, COLOR_PAIR(GRAY_DEFAULT));
		wattron(zoomBox, A_BOLD);
		wborder(zoomBox, 0, 0, 0, 0, ACS_ULCORNER, ACS_URCORNER, ACS_LTEE, ACS_BTEE);
		wrefresh(zoomBox);

	/* searchBox */
		werase(searchBox);
		wattron(searchBox, COLOR_PAIR(BLUE_DEFAULT));
		wattron(searchBox, A_BOLD);		
		wborder(searchBox, ' ', ' ', 0, 0, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
		wrefresh(searchBox);
		wattron(searchWindow, COLOR_PAIR(BLUE_DEFAULT));
		wattron(searchWindow, A_BOLD);

	// MIDI_state_window
		wattron( MIDI_state_window, A_BOLD );
		wattron( MIDI_state_window, A_BLINK );

	/* DIALOG_WINDOW */
		werase(ventana[DIALOG_WINDOW]);
		wattron(ventana[DIALOG_WINDOW], COLOR_PAIR(WHITE_DEFAULT));
		wattron(ventana[DIALOG_WINDOW], A_BOLD);
		wborder(ventana[DIALOG_WINDOW], 0, 0, 0, 0, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
		mvwprintw(ventana[DIALOG_WINDOW], 0, 5, " Load / Save ");

	/* DIALOG_WINDOW */
		werase(ventana[INPUT_BOX]);
		wattron(ventana[INPUT_BOX], COLOR_PAIR(GRAY_DEFAULT));
		wattron(ventana[INPUT_BOX], A_BOLD);
	
	return;
}/*}}}*/

void tint_lcd(const short int mode) {/*{{{*/
	switch(mode) {
		case COMBINATION:
			wattron(lcdWindow, COLOR_PAIR(5));
			break;
		case SEQUENCER:
			wattron(lcdWindow, COLOR_PAIR(3));
			break;
	}

	wrefresh( lcdWindow );
	return;
}/*}}}*/

void update_popups() noexcept {/*{{{*/
	update_panels();
	doupdate();
}/*}}}*/

auto get_y_pixels() noexcept -> std::int16_t {/*{{{*/
	return screen_height;
}/*}}}*/
