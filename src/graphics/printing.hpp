#ifndef PRINTING_HPP
#define PRINTING_HPP

#include <bits/stdint-intn.h>
#include <ncurses.h>
#include "../data/databases.hpp"
#include "data/playlist.hpp"


void print_displayTable(	WINDOW *window,
							System *displayTable[],
							const int32_t top, const short resultRows, 
							const int index,
							short int winMode);

void print_playlist(	WINDOW *window, System playlistTable[], const int32_t top,
		const short resultRows, const int indexA, const int indexB, short int winMode );

void print_playlist( WINDOW *_WindowPtr, const int32_t &_Top, const int &_IndexA,
					const int &_IndexB,
					Playlist &_PlaylistRef, const short int &_WinMode ) noexcept;

void lcd(	WINDOW *window, 
			const short int yPos, const short int xPos, const short int limit,
			const bool cursor, const char cadena[]);

void print_search(WINDOW *window, char cadena[]);

void print_lcd(WINDOW *window, System *linea );

void print_computer(WINDOW *window, const short int oxygen, const short int mode, const Variation var);

void print_zoom(WINDOW *window, System *linea);

void print_MIDI_state( WINDOW *&_Ventana, const enum Switch &_State ) noexcept;
#endif
