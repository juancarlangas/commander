#include <ncurses.h>
#include "common.h"
#include "colors.h"
#include "Windows/zoom.h"
#include "Windows/bannerBox.h"

#include <stdlib.h>

WINDOW 	*searchBox,		*searchWindow,
		*lcdBox,		*lcdWindow,
		*zoomBox,		*zoomWindow,
		*computerBox,	*computerWindow,
		*displayBox,	*displayWindow,
		*playlistBox,	*playlistWindow,
		*digitsBox, 	*digitsWindow;

WINDOW 	*ventana[2];
PANEL 	*panel[2];

BannerBox bannerBox;

short int x, y;
short displayShowResults, playlistShowResults;	

//Display display(y * 44 / 200, x * 141 / 200, y * 26 / 200, x * 0 / 200);

/******************************************************* init_ncurses *************************************************/
short int init_ncurses(void)

{
	initscr();
	raw();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(FALSE);
	set_escdelay(3);
	
	getmaxyx(stdscr, y, x);

	displayShowResults  = y * 54 / 100 - 4;
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

	
	searchBox 		= newwin(6, 				x * 140 / 200,     	y * 84 / 100,     	x * 0 / 200	 	);
	searchWindow 	= newwin(4, 				x * 140 / 200 - 2, 	y * 84 / 100 + 1, 	x * 0 / 200 + 1	);

	computerBox		= newwin(1,					x * 60 / 200,     	y * 8 / 100,     	x * 150 / 200	);
	computerWindow 	= newwin(1,					x * 60 / 200 - 2, 	y * 8 / 100, 		x * 150 / 200 + 1);

	//lcdBox 			= newwin(6,	    			x * 98 / 200,     	0,					x * 50 / 200	);
	lcdWindow 		= newwin(4, 				x * 98 / 200 - 2, 	0,  				x * 50 / 200 + 1);
	
	
	playlistBox		= newwin(y * 70 / 100, 		x * 60 / 200, 		y * 32 / 100,	   	x * 70 / 100  	);
	playlistWindow 	= newwin(y * 70 / 100 - 3, 	x * 60 / 200 - 2, 	y * 32 / 100 + 2, 	x * 70 / 100 + 1);
	
	displayBox 		= newwin(y * 54 / 100,		x * 141 / 200,     	y * 32 / 100,     	x * 0 / 100	 	);
	displayWindow 	= newwin(y * 54 / 100 - 3,	x * 141 / 200 - 2, 	y * 32 / 100 + 2, 	x * 0 / 100 + 1	);

	zoomBox 		= newwin(y * 36 / 200, 		x * 120 / 200, 		y * 36 / 200, 		x * 80 / 200		);		
	zoomWindow 		= newwin(y * 36 / 200 - 2, 	x * 120 / 200 - 2, 	y * 36 / 200 + 1, 	x * 80 / 200 + 1	);		
	
	ventana[DIALOG_WINDOW]	= newwin(3,			x * 40 / 100,		y * 40 / 100,		x * 30 / 100	);
	ventana[INPUT_BOX]		= newwin(1,			x * 40 / 100 - 2,	y * 40 / 100 + 1,	x * 30 / 100 + 1);

	panel[DIALOG_WINDOW]	= new_panel(ventana[DIALOG_WINDOW]);
	panel[INPUT_BOX] 		= new_panel(ventana[INPUT_BOX]);
	
	hide_panel(panel[DIALOG_WINDOW]);
	hide_panel(panel[INPUT_BOX]);
	
	update_panels();
	doupdate();
	
	return y;
}

/********************************* draw_windows ************************/
void draw_windows(void)
{
	/* lcdBox */
		wattron(lcdBox, COLOR_PAIR(GREEN_DEFAULT));
		wattron(lcdBox, A_BOLD);
		wborder(lcdBox, ' ', ' ', 0, 0, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
		wrefresh(lcdBox);

	/* computerBox */
		wattron(computerBox, COLOR_PAIR(GREEN_DEFAULT));
		wattron(computerBox, A_BOLD);
		wborder(computerBox, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
		wrefresh(computerBox);

	/* zoomBox*/
		wattron(zoomBox, COLOR_PAIR(GRAY_DEFAULT));
		wattron(zoomBox, A_BOLD);
		wborder(zoomBox, 0, 0, 0, 0, ACS_ULCORNER, ACS_URCORNER, ACS_BTEE, ACS_RTEE);
		wrefresh(zoomBox);

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
		mvwprintw(displayBox, 1, 26, " SONG ");
		mvwprintw(displayBox, 1, 32, " ARTIST ");
		mvwprintw(displayBox, 1, 59, " GENRE ");
		mvwprintw(displayBox, 1, 76, " KEYWORDS ");
		wrefresh(displayBox);

	/* zoomBox*/
		wattron(zoomBox, COLOR_PAIR(GRAY_DEFAULT));
		wattron(zoomBox, A_BOLD);
		wborder(zoomBox, 0, 0, 0, 0, ACS_ULCORNER, ACS_URCORNER, ACS_BTEE, ACS_RTEE);
		wrefresh(zoomBox);

	//bannerBox.draw(PLAYLISTBOX);

	/* searchBox */
		wattron(searchBox, COLOR_PAIR(BLUE_DEFAULT));
		wattron(searchBox, A_BOLD);		
		wborder(searchBox, ' ', ' ', 0, 0, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
		wrefresh(searchBox);
		wattron(searchWindow, COLOR_PAIR(BLUE_DEFAULT));
		wattron(searchWindow, A_BOLD);
	
	/* DIALOG_WINDOW */
		wattron(ventana[DIALOG_WINDOW], COLOR_PAIR(WHITE_DEFAULT));
		wattron(ventana[DIALOG_WINDOW], A_BOLD);
		wborder(ventana[DIALOG_WINDOW], 0, 0, 0, 0, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
		mvwprintw(ventana[DIALOG_WINDOW], 0, 5, " Load / Save ");

	

	/* DIALOG_WINDOW */
		wattron(ventana[INPUT_BOX], COLOR_PAIR(GRAY_DEFAULT));
		wattron(ventana[INPUT_BOX], A_BOLD);
	
	/*Dialog dialog_box;*/

	return;
}

/*********************** tint_lcd ****************************************/
void tint_lcd(const short int mode)
{
	switch(mode) {
		case COMBINATION:
			wattron(lcdWindow, COLOR_PAIR(5));
			break;
		case SEQUENCER:
			wattron(lcdWindow, COLOR_PAIR(3));
			break;
	}

	return;
}