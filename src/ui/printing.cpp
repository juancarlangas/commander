#include "printing.hpp"
#include <ncurses.h>
#include <ctype.h>
#include "../common/common.hpp"
#include "colors.hpp"
#include "../data/catalog.hpp"
#include "data/playlist.hpp"

extern const char *get_digit(char, short int);

extern WINDOW *debugWindow;
extern short int x, y;
extern short int displayShowResults, playlistShowResults;

void print_displayTable(WINDOW *window,/*{{{*/
						Performance *displayTable[],
						const int32_t top, const short resultRows, 
						const int index,
						short int winMode) {
	int32_t i;
	short int row = 0;

	wclear(window);
	
	for (i = top; ( i <= top + displayShowResults - 1) && (i <= resultRows - 1); i++) {
		switch (displayTable[i]->metadata.mood[0]) {
			case 'L':
				wattron(window, COLOR_PAIR(MAGENTA_DEFAULT));
				break;
			case 'C':
				wattron(window, COLOR_PAIR(CYAN_DEFAULT));
				break;
			case 'B':
				wattron(window, COLOR_PAIR(YELLOW_DEFAULT));
				break;
			case 'A':
				wattron(window, COLOR_PAIR(RED_DEFAULT));
				break;
			case 'S':
				wattron(window, COLOR_PAIR(WHITE_DEFAULT));
				break;
			default:
				break;
		}

		if (i == index && winMode == 1)
			wattron(window, A_REVERSE);
		mvwprintw( window, 1 + row++, 0, "%40s  %-30s  %-20s  %-23s", 
				displayTable[i]->metadata.title.substr( 0, 31 ).c_str(),
				displayTable[i]->metadata.artist.substr( 0, 20 ).c_str(),
				displayTable[i]->metadata.genre.substr( 0, 15 ).c_str(),
				displayTable[i]->metadata.keyword.substr( 0, 14 ).c_str() );
		wattroff(window, A_REVERSE);
		wattroff(window, A_BOLD);
	}

	wrefresh(window);
	
	return;
}/*}}}*/

void print_playlist( WINDOW *_WindowPtr, const int32_t &_Top, const int &_Index,/*{{{*/
					Playlist &_PlaylistRef, const short int &_WinMode ) noexcept
{
	short int row = 1;

	wclear( _WindowPtr );
	
	for ( int32_t i = _Top;
		( i < _Top + playlistShowResults ) && ( i < _PlaylistRef.get_n_pistas() );
		++i) {
		if ( i == _Index and _WinMode == 2 ) { /*resaltado*/
			wattron( _WindowPtr, COLOR_PAIR(WHITE_DEFAULT) );
			wattron( _WindowPtr, A_REVERSE);
			mvwprintw( _WindowPtr, row, 1, "%2d. ", i + 1);
			mvwprintw( _WindowPtr, row++, 5, "%-30s", _PlaylistRef.get_titulo( i ).c_str() );
			wattroff( _WindowPtr, A_REVERSE);
		}
		else {
			wattron( _WindowPtr, COLOR_PAIR(GRAY_DEFAULT));
			wattron( _WindowPtr, A_BOLD);
			mvwprintw( _WindowPtr, row, 1, "%2d. ", i + 1);
			wattron( _WindowPtr, COLOR_PAIR(WHITE_DEFAULT ) );
			mvwprintw( _WindowPtr, row++, 5, "%-30s", _PlaylistRef.get_titulo( i ).c_str() );
		}
	}

	wrefresh( _WindowPtr );
	
	return;
}/*}}}*/

void lcd(	WINDOW *window, /*{{{*/
			const short int yPos, const short int xPos, const short int limit,
			const bool cursor, const char cadena[])
{
	short int i = 0, j;
	
	while(cadena[i] != '\0' && i <= limit) {
		for (j = 0; j <= 2; j++)
			mvwprintw(window, yPos + j, xPos + i * 4, "%s", get_digit(toupper(cadena[i]), j));
		i++;
	}
	
	/*Cursor*/
	if (cursor == TRUE) {
		wattron(window, A_BLINK);
		mvwprintw(window, yPos + 2, xPos + i * 4, "___");
		wattroff(window, A_BLINK);
	}

	wrefresh(window);

	return;
}/*}}}*/

void print_search(WINDOW *window, char cadena[])/*{{{*/
{
	wclear(window);
	lcd(window, 0, 2, 16, TRUE, cadena);
	wrefresh(window);

	return;	
}/*}}}*/

void print_lcd(WINDOW *window, Performance *linea )/*{{{*/
{
	short int k = 0, yPos;
	
	wclear(window);
	wattron(window, COLOR_PAIR(GREEN_DEFAULT));
	wattron(window, A_BOLD);

	k = 0;
	while (linea->metadata.title[k] != '\0' && k <= 19) {
		for (yPos = 0; yPos <= 2; yPos++)
			mvwprintw(window, yPos, 2 + k * 4, "%s", get_digit(toupper(linea->metadata.title[k]), yPos));
		k++;
	}

	wrefresh(window);

	return;
}/*}}}*/

void print_computer(WINDOW *window, const short int oxygen, const short int mode) {/*{{{*/
	wattron(window, A_BOLD);

	switch (oxygen) {
		case CONNECTED:
			wattron(window, COLOR_PAIR(YELLOW_DEFAULT));
			mvwprintw(window, 0, 2, "Oxygen");
			break;

		case DISCONNECTED:
			wattron(window, COLOR_PAIR(GRAY_DEFAULT));
			mvwprintw(window, 0, 2, "Oxygen");
			break;
	}
		
	switch (mode) {
		case COMBINATOR:
			wattron(window, COLOR_PAIR(RED_DEFAULT));
			mvwprintw(window, 0, 10, "CMB");
			wattron(window, COLOR_PAIR(GRAY_DEFAULT));
			mvwprintw(window, 0, 14, "SEQ");
			break;
		case SEQUENCER:
			wattron(window, COLOR_PAIR(GRAY_DEFAULT));
			mvwprintw(window, 0, 10, "CMB");
			wattron(window, COLOR_PAIR(RED_DEFAULT));
			mvwprintw(window, 0, 14, "SEQ");
			break;
	}

	/*
	switch (var) {
		case VAR1:
			wattron(window, COLOR_PAIR(RED_DEFAULT));
			mvwprintw(window, 0, 21, "VAR1");
			wattron(window, COLOR_PAIR(GRAY_DEFAULT));
			mvwprintw(window, 0, 26, "VAR2");
			break;
		case VAR2:
			wattron(window, COLOR_PAIR(GRAY_DEFAULT));
			mvwprintw(window, 0, 21, "VAR1");
			wattron(window, COLOR_PAIR(RED_DEFAULT));
			mvwprintw(window, 0, 26, "VAR2");
			break;
	}
	
	wrefresh(window);
	*/

	return;
}/*}}}*/

void print_zoom(WINDOW *window, Performance *linea)/*{{{*/
{
	//static char message[50];

	wclear(window);
	
	wattron(window, COLOR_PAIR(WHITE_DEFAULT));
	/*switch ((linea->section)[0]) {
		case 'L':
			wattron(window, COLOR_PAIR(MAGENTA_DEFAULT));
			break;
		case 'C':
			wattron(window, COLOR_PAIR(CYAN_DEFAULT));
			break;
		case 'B':
			wattron(window, COLOR_PAIR(YELLOW_DEFAULT));
			break;
		case 'A':
			wattron(window, COLOR_PAIR(RED_DEFAULT));
			break;
		case 'S':
			wattron(window, COLOR_PAIR(WHITE_DEFAULT));
			break;
		default:
			wattron(window, COLOR_PAIR(WHITE_DEFAULT));
			break;
	}*/

		wattron(window, A_BOLD);
		
		//sprintf(message, "%c-%03hd %s", linea->bnk2, linea->num2, linea->title);

		lcd(window, 1, 2, 18, FALSE, linea->metadata.title.c_str());

		mvwprintw(window, 0, 2, "%c-%03d", linea->patch.bnk + 65, linea->patch.num);
		wrefresh(window);
	
	return;
}/*}}}*/

void print_MIDI_state( WINDOW *&_Ventana, const enum Switch &_State ) noexcept {/*{{{*/
	wclear( _Ventana );

	if ( _State == Switch::OFF ) {
		wattron( _Ventana, COLOR_PAIR( YELLOW_DEFAULT ) );
		wattron( _Ventana, A_BLINK );
	}
	else {
		wattron( _Ventana, COLOR_PAIR( GREEN_DEFAULT ) );
		wattroff( _Ventana, A_BLINK );
	}
	mvwaddstr( _Ventana, 0, 0, "MIDI" );
	wrefresh( _Ventana );
}/*}}}*/
