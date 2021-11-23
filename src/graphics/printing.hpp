#ifndef PRINTING_HPP
#define PRINTING_HPP

#include <bits/stdint-intn.h>
#include <ncurses.h>
#include "../data/databases.hpp"


void print_displayTable(	WINDOW *window,
							System *displayTable[],
							const int32_t top, const short resultRows, 
							const int index,
							short int winMode);

void print_playlist(	WINDOW *window, System playlistTable[], const int32_t top,
		const short resultRows, const int indexA, const int indexB, short int winMode );

void lcd(	WINDOW *window, 
			const short int yPos, const short int xPos, const short int limit,
			const bool cursor, const char cadena[]);

void print_search(WINDOW *window, char cadena[]);

void print_lcd(WINDOW *window, System *linea );

void print_computer(WINDOW *window, const short int oxygen, const short int mode, const Variation var);

void print_zoom(WINDOW *window, System *linea);

#endif
