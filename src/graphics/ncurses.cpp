#include "common/common.hpp"
#include "graphics/colors.hpp"
#include "graphics/windows.hpp"
#include "graphics/ncurses.hpp"
#include "graphics/orchestra.hpp"

#include <ncurses.h>
#include <stdlib.h>

short int x, y;
short displayShowResults, playlistShowResults;	

WINDOW 	*searchBox,		*searchWindow,
		*lcdWindow,
		*zoomBox,		*zoomWindow,
		*displayBox,	*displayWindow,
		*playlistBox,	*playlistWindow,
		*MIDI_state_window;

WINDOW 	*ventana[2];
PANEL 	*panel[2];

Orchestra orquestacion;

void init_screen() noexcept/*{{{*/
{
	initscr();
	raw();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(FALSE);
	set_escdelay(3);
}/*}}}*/

void end_screen() noexcept/*{{{*/
{
	set_escdelay( 0 );
	curs_set( TRUE );
	keypad( stdscr, FALSE );
	echo();
	noraw();
	endwin();
}/*}}}*/

short int init_ncurses(void)/*{{{*/
{
	init_screen();

	getmaxyx(stdscr, y, x);
	displayShowResults  = y * 24;
	playlistShowResults = y * 70 / 100 - 4;


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
	
	refresh();

	searchBox 		= newwin(5, 				x * 140 / 200,     	y * 90 / 100,     	x * 0 / 200	 	);
	searchWindow 	= newwin(3, 				x * 140 / 200 - 2, 	y * 90 / 100 + 1, 	x * 0 / 200 + 1	);

	// lcdBox		= newwin(y * 38 / 200,	 	x * 98 / 200,     	y * 36 / 200,		x * 102 / 200	);
	lcdWindow 		= newwin(y * 30 / 200 - 2, 	x * 98 / 200 - 2, 	y * 20 / 200 + 1,	x * 98 / 200 + 1);
	
	playlistBox		= newwin(33, 		x * 60 / 200, 		9,	   	x * 141 / 200  	);
	playlistWindow 	= newwin(33 - 3, 	x * 60 / 200 - 2, 	9 + 2, 	x * 141 / 200 + 1);
	
	displayBox 		= newwin(28,		x * 142 / 200,     	9,     	x * 0 / 100	 	);
	displayWindow 	= newwin(28 - 3,	x * 142 / 200 - 2, 	9 + 2, 	x * 0 / 100 + 1	);

	zoomBox 		= newwin(7, 		x * 98 / 200, 		3, 		x * 0 / 200	);		
	zoomWindow 		= newwin(7 - 2, 	x * 98 / 200 - 2, 	3 + 1, 	x * 0 / 200 + 1	);
	
	MIDI_state_window = newwin( 1, 4, 2, x * 180 / 200 );

	// Salvar / cargar playlist
	ventana[DIALOG_WINDOW]	= newwin(3,			x * 40 / 100,		y * 40 / 100,		x * 30 / 100	);
	ventana[INPUT_BOX]		= newwin(1,			x * 40 / 100 - 2,	y * 40 / 100 + 1,	x * 30 / 100 + 1);

	panel[DIALOG_WINDOW]	= new_panel(ventana[DIALOG_WINDOW]);
	panel[INPUT_BOX] 		= new_panel(ventana[INPUT_BOX]);
	
	hide_panel(panel[DIALOG_WINDOW]);
	hide_panel(panel[INPUT_BOX]);

	orquestacion.init( y * 180 / 200, x * 180 / 200, y * 20 / 200, x * 10 / 200 );

	refresh();

	return y;
}/*}}}*/
	
void draw_windows(void)
{/*{{{*/
	/* playlistBox */
		wattron(playlistBox, COLOR_PAIR(GRAY_DEFAULT));
		wattron(playlistBox, A_BOLD);
		wborder(playlistBox, 0, 0, 0, 0, ACS_TTEE, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
		mvwprintw(playlistBox, 1, 12, " PlayList ");
		wrefresh(playlistBox);

	/* displayBox */
		wattron(displayBox, COLOR_PAIR(GRAY_DEFAULT));
		wattron(displayBox, A_BOLD);
		wborder(displayBox, 0, 0, 0, 0, ACS_LTEE, ACS_TTEE, ACS_LLCORNER, ACS_RTEE);
		mvwprintw(displayBox, 1, 36, " SONG ");
		mvwprintw(displayBox, 1, 41, " ARTIST ");
		mvwprintw(displayBox, 1, 72, " GENRE ");
		mvwprintw(displayBox, 1, 96, " KEYWORDS ");
		wrefresh(displayBox);

	/* zoomBox*/
		wattron(zoomBox, COLOR_PAIR(GRAY_DEFAULT));
		wattron(zoomBox, A_BOLD);
		wborder(zoomBox, 0, 0, 0, 0, ACS_ULCORNER, ACS_URCORNER, ACS_LTEE, ACS_BTEE);
		wrefresh(zoomBox);

	/* searchBox */
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
		wattron(ventana[DIALOG_WINDOW], COLOR_PAIR(WHITE_DEFAULT));
		wattron(ventana[DIALOG_WINDOW], A_BOLD);
		wborder(ventana[DIALOG_WINDOW], 0, 0, 0, 0, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
		mvwprintw(ventana[DIALOG_WINDOW], 0, 5, " Load / Save ");

	/* DIALOG_WINDOW */
		wattron(ventana[INPUT_BOX], COLOR_PAIR(GRAY_DEFAULT));
		wattron(ventana[INPUT_BOX], A_BOLD);
	
	return;
}/*}}}*/

void tint_lcd(const short int mode)
{/*{{{*/
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

void update_popups() noexcept
{/*{{{*/
	update_panels();
	doupdate();
}/*}}}*/
