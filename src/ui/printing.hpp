#ifndef PRINTING_HPP
#define PRINTING_HPP

#include <bits/stdint-intn.h>
#include <ncurses.h>
#include "data/Catalog.hpp"
#include "data/Playlist.hpp"


void print_displayTable(	WINDOW *window,
							Performance *displayTable[],
							const int32_t top, const short resultRows, 
							const int index,
							short int winMode);

void print_playlist( WINDOW *_WindowPtr, const int32_t &_Top, const int &_IndexA,
					Playlist &_PlaylistRef, const short int &_WinMode ) noexcept;

void lcd(	WINDOW *window, 
			const short int yPos, const short int xPos, const short int limit,
			const bool cursor, const char cadena[]);

void print_search(WINDOW *window, char cadena[]);

void print_lcd(WINDOW *window, Performance *linea );

void print_computer(WINDOW *window, const short int oxygen, const short int mode);

void print_zoom(WINDOW *window, Performance *linea);

void print_MIDI_state( WINDOW *&_Ventana, const enum Switch &_State ) noexcept;
#endif
