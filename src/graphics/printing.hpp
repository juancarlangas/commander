#ifndef PRINTING_HPP
#define PRINTING_HPP

#include <bits/stdint-intn.h>
#include <ncurses.h>
#include "../data/databases.hpp"


void print_displayTable(	WINDOW *window,
							System *displayTable[],
							const unsigned int top, const short resultRows, 
							const int index,
							short int winMode);

void print_playlist(	WINDOW *window,
						System playlistTable[],
						const unsigned int top, const short resultRows, 
						const int indexA, const int indexB,
						const short int mode, short int winMode		);

void lcd(	WINDOW *window, 
			const short int yPos, const short int xPos, const short int limit,
			const short int color, const bool cursor,
			const char cadena[]);

void print_search(WINDOW *window, char cadena[]);

void print_lcd(WINDOW *window, System *linea, const Variation var );

void print_computer(WINDOW *window, const short int oxygen, const short int mode, const Variation var);

void print_mode(WINDOW *window, const short int mode );

void print_zoom(WINDOW *window, System *linea);


#endif

