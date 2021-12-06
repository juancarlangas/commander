#include "main.hpp"
#include "common/common.hpp"
#include "common/matroska.hpp"
#include "data/databases.hpp"
#include "graphics/colors.hpp"
#include "graphics/ncurses.hpp"
#include "graphics/windows.hpp"
#include <cstdlib>
#include <panel.h>

int main()
{
	// ncurses{{{
	y = init_ncurses();
	draw_windows();
	short int updateWindow[8] = {0};/*}}}*/

	// File System{{{
	const char *homedir;
	if ((homedir = getenv("HOME")) == NULL)
		homedir = getpwuid(getuid())->pw_dir;

   	char directory[50];
   	sprintf(directory, "%s/.commander", homedir);/*}}}*/

	// Databases{{{
	Database dBase[1];	// C++ Object (1 for combs, 1 for seqs)

	int nRows[1];
    const std::string config_directory{ directory };

	nRows[COMBINATIONS] = Files::contar_lineas( config_directory + "/combinations-2.csv" );

    dBase[COMBINATIONS].cargar( config_directory + "/combinations-2.csv" );/*}}}*/

	// Tables{{{
	System **displayTable = new System *[ nRows[COMBINATIONS] ]();
	System *playlistTable = new System [ nRows[COMBINATIONS] ](), *buffer;

	int 	dRows, 		dTop  = 0, 	dIndex   = 0,
			plRows = 0, plTop = 0, 	plIndexA = 0, plIndexB = 0;
	char 	keyword[LONG_STRING] = "\0";
	int caracter;
	short int charIndex = 0;

	int i;
	short int k;/*}}}*/

	// System
	short int	mode = COMBINATOR,/*{{{*/
				winMode = MODE_DISPLAY;
	enum matroska command = BEGIN;/*}}}*/

	// Keyboards
	Keyboard keyboard;/*{{{*/
	keyboard.set_name("X50");

	orquestacion.link_MIDI_device( &keyboard );/*}}}*/

	do {
		for (i = LCD; i <= ZOOM; i++)
			updateWindow[i] = false;

		switch (command) {
			case BEGIN:/*{{{*/
				llenado_displayTable(
					displayTable, dBase[COMBINATIONS].base, nRows[COMBINATOR], keyword, &dRows );
				plRows = load_playlist(playlistTable, "default");

				if (nRows[COMBINATOR] > 0)
					buffer = dBase[COMBINATIONS].base;

				updateWindow[LCD]		= true;
				updateWindow[SEARCH] 	= true;
				updateWindow[DISPLAY]	= true;
				updateWindow[PLAYLIST] 	= true;
				updateWindow[COMPUTER] 	= true;
				updateWindow[DIGITS]	= true;
				updateWindow[ZOOM]		= true;
				updateWindow[MIDI_STATE] = true;

				break;/*}}}*/

			case CARGAR_ESPECIFICO :/*{{{*/
				dBase[ COMBINATIONS ].cargar_especifico( config_directory + "/combinations-2.csv",
						buffer - dBase[ COMBINATIONS ].base );
				buffer = displayTable[ dIndex ];
				// keyboard.select_page( TIMBRE );
				keyboard.set_buffer( *buffer );
				keyboard.dump_variation();
				break;/*}}}*/

			case SET_MODE:
				for (k = 0; k <= LONG_STRING - 1; k++)/*{{{*/
					keyword[k] = '\0';

				if (mode == COMBINATOR)
					mode = SEQUENCER;
				else
					mode = COMBINATOR;

				llenado_displayTable(displayTable, dBase[mode].base,  nRows[mode],  keyword, &dRows);

				charIndex = 0;
				dTop   = 0;
				dIndex = 0;
				winMode = MODE_DISPLAY;

				updateWindow[SEARCH] 	= true;
				updateWindow[DISPLAY]	= true;
				updateWindow[COMPUTER] 	= true;
				updateWindow[DIGITS]	= true;
				updateWindow[ZOOM]		= true;

				break;/*}}}*/

			case ESCAPE:
				for (k = 0; k <= LONG_STRING - 1; k++)/*{{{*/
					keyword[k] = '\0';

				llenado_displayTable(displayTable, dBase[mode].base, nRows[mode], keyword, &dRows);

				charIndex = 0;
				dTop = 0;
				dIndex = 0;

				if (winMode == MODE_PLAYLIST) {
					winMode = MODE_DISPLAY;
					updateWindow[PLAYLIST] = true;
				}
				updateWindow[LCD] 	  = true;
				updateWindow[SEARCH]  = true;
				updateWindow[DISPLAY] = true;
				updateWindow[ZOOM]	  = true;

				break;/*}}}*/

			case PREV_VARIATION:
				keyboard.prev_variation();/*{{{*/
				break;/*}}}*/

			case NEXT_VARIATION :
				keyboard.next_variation();/*{{{*/
				break;/*}}}*/
				
			case INTRO:
				switch (winMode) {/*{{{*/
					case MODE_DISPLAY:
						buffer = displayTable[ dIndex ];
						break;
					case MODE_PLAYLIST:
						buffer = &playlistTable[ plIndexA ];
						//avance carro
						if (plIndexB < plRows - 1) {
							plIndexB++;
							plIndexA = plIndexB;
						}
						updateWindow[PLAYLIST] = true;
						updateWindow[ZOOM] = true;

						break;
				}

				keyboard.set_program( *buffer );

				updateWindow[LCD] = true;

				break;/*}}}*/

			case CHANGE_WINDOW:
/*{{{*/
				if (winMode == MODE_DISPLAY && plRows > 0) {
					winMode = MODE_PLAYLIST;
					if (plIndexA < plTop && plIndexB < plTop) {// <-- Que no se quede volando
						plIndexA = plTop;
						plIndexB = plTop;
					}
				}
				else if (winMode == MODE_PLAYLIST)
					winMode = MODE_DISPLAY;

				updateWindow[DISPLAY] 	= true;
				updateWindow[PLAYLIST] 	= true;
				updateWindow[ZOOM]		= true;

				break;/*}}}*/

			case MOVE_INDEX:
/*{{{*/
				if (caracter == KEY_UP || caracter == KEY_LEFT)
					switch (winMode) {
						case MODE_DISPLAY:
							decrease_index( &dTop, &dIndex );
							updateWindow[DISPLAY] = true;
							break;
						case MODE_PLAYLIST:
							decrease_index(&plTop, &plIndexB );
							plIndexA = plIndexB;
							updateWindow[PLAYLIST] = true;
							break;
					}

				else if (caracter == KEY_DOWN || caracter == KEY_RIGHT)
					switch (winMode) {
						case MODE_DISPLAY:
							increase_index(&dTop, dRows, &dIndex, winMode);
							updateWindow[DISPLAY] = true;
							break;
						case MODE_PLAYLIST:
							increase_index(&plTop, plRows, &plIndexB, winMode);
							plIndexA = plIndexB;
							updateWindow[PLAYLIST] = true;
							break;
					}

				updateWindow[DIGITS] = true;
				updateWindow[ZOOM]	= true;

				break;/*}}}*/

			case ADD:
				playlistTable[(plRows)++] = *displayTable[dIndex];/*{{{*/
				if (plRows > plTop + playlistShowResults)
					(plTop)++;

				save_playlist(playlistTable, plRows, "default");


				updateWindow[PLAYLIST] = true;

				break;/*}}}*/

			case SUPR:
				for (i = plIndexA; i <= plRows - 1; i++)  //succión{{{
					playlistTable[i] = playlistTable[i + 1];
				if (plIndexA == plRows - 1) { //fin de lista
					decrease_index(&plTop, &plIndexA );
					(plIndexB = plIndexA);
				}
				(plRows)--;

				save_playlist(playlistTable, plRows, "default");

				if (plRows == 0) { //cambio
					winMode = MODE_DISPLAY;
					updateWindow[DISPLAY] = true;
				}
				updateWindow[LCD] 	   = true;
				updateWindow[PLAYLIST] = true;
				updateWindow[ZOOM]	= true;

				break;/*}}}*/

			case DEL:
/*{{{*/
				charIndex--;
				keyword[charIndex] = '\0';

				llenado_displayTable(displayTable, dBase[mode].base, nRows[mode], keyword, &dRows);
				
				dTop = 0;
				dIndex = 0;

				updateWindow[0] = 1;
				updateWindow[1] = 1;
				updateWindow[2] = 1;
				updateWindow[ZOOM] = true;

				if (winMode == MODE_PLAYLIST)
					winMode = MODE_DISPLAY;

				break;/*}}}*/

			case DRAG_UP: case DRAG_DOWN:
/*{{{*/
				if (winMode == MODE_PLAYLIST && caracter == 566 && plIndexB > 0) {
					korg_drag(playlistTable, plRows, plIndexB, plIndexB, caracter);
					decrease_index(&plTop, &plIndexB );
					plIndexA = plIndexB;
					save_playlist(playlistTable, plRows, "default");
					updateWindow[PLAYLIST] = true;
				}

				else if (winMode == MODE_PLAYLIST && caracter == 525 && plIndexB < plRows - 1) {
					korg_drag(playlistTable, plRows, plIndexB, plIndexB, caracter);
					increase_index(&plTop, plRows, &plIndexB, winMode);
					plIndexA = plIndexB;
					save_playlist(playlistTable, plRows, "default");
					updateWindow[PLAYLIST] = true;
				}
				
				break;/*}}}*/

			case READ_CHAR:
/*{{{*/
				//agregate
				keyword[charIndex++] = caracter;
				keyword[charIndex] = '\0';

				if (keyword[charIndex - 1] != -61) { // saltamos acentos y tildes
					charIndex = no_accent(keyword, keyword); //clean

					llenado_displayTable(	displayTable, dBase[mode].base,
											nRows[mode], keyword, &dRows	);

					/*
					 * Esta parte se supone que era un hack para que no imprimiera nada
					 * pero resultó contraproducente pues el -1 sí habilitaba la impresión.
					if (dRows == 0)
						dTop = dIndex = -1;
					else
						dTop = dIndex = 0;
					*/

					if (winMode == MODE_PLAYLIST) {
						winMode = MODE_DISPLAY;
						updateWindow[PLAYLIST] = true;
					}

					if (keyword[0] != ':' || charIndex == 1)
						updateWindow[ZOOM] = true;

					updateWindow[LCD] 	  = true;
					updateWindow[SEARCH]  = true;
					updateWindow[DISPLAY] = true;

				}
				break;/*}}}*/

			case SAVE_PLAYLIST:
/*{{{*/
				show_panel(panel[DIALOG_WINDOW]);
				show_panel(panel[INPUT_BOX]);
				update_panels();
				doupdate();

				echo();
				wmove(ventana[INPUT_BOX], 0, 0);
				curs_set(true);
				noraw();

					char file_name[40];
					wscanw(ventana[INPUT_BOX], "%s", file_name);
					save_playlist(playlistTable, plRows, file_name);

				noecho();
				wclear(ventana[INPUT_BOX]);
				curs_set(false);
				raw();

				hide_panel(panel[DIALOG_WINDOW]);
				hide_panel(panel[INPUT_BOX]);
				update_panels();
				doupdate();

				for (k = 0; k <= LONG_STRING - 1; k++)
					keyword[k] = '\0';

				llenado_displayTable(displayTable, dBase[mode].base,  nRows[mode],  keyword, &dRows);

				charIndex = 0;
				dTop   = 0;
				dIndex = 0;
				winMode = MODE_DISPLAY;

				updateWindow[SEARCH] 	= true;
				updateWindow[DISPLAY]	= true;
				updateWindow[ZOOM]		= true;
				updateWindow[PLAYLIST]	= true;

				break;/*}}}*/

			case LOAD_PLAYLIST:
/*{{{*/
				show_panel(panel[DIALOG_WINDOW]);
				show_panel(panel[INPUT_BOX]);
				update_panels();
				doupdate();

				echo();
				wmove(ventana[INPUT_BOX], 0, 0);
				curs_set(true);
				noraw();
				{
					char file_name[40];
					wscanw(ventana[INPUT_BOX], "%s", file_name);
					plRows = load_playlist(playlistTable, file_name);
				}
				noecho();
				wclear(ventana[INPUT_BOX]);
				curs_set(false);
				raw();

				hide_panel(panel[DIALOG_WINDOW]);
				hide_panel(panel[INPUT_BOX]);
				update_panels();
				doupdate();

				save_playlist(playlistTable, plRows, "default");

				for (k = 0; k <= LONG_STRING - 1; k++)
					keyword[k] = '\0';

				llenado_displayTable(displayTable, dBase[mode].base,  nRows[mode],  keyword, &dRows);

				charIndex = 0;
				dTop   = 0;
				dIndex = 0;
				winMode = MODE_DISPLAY;

				updateWindow[SEARCH] 	= true;
				updateWindow[DISPLAY]	= true;
				updateWindow[ZOOM]		= true;
				updateWindow[PLAYLIST]	= true;

				break;
/*}}}*/

			case CLEAR_PLAYLIST:
/*{{{*/
				plRows = 0;
				save_playlist(playlistTable, plRows, "default");

				for (k = 0; k <= LONG_STRING - 1; k++)
					keyword[k] = '\0';

				llenado_displayTable(displayTable, dBase[mode].base,  nRows[mode],  keyword, &dRows);

				if (winMode == MODE_PLAYLIST)
					winMode = MODE_DISPLAY;

				charIndex = 0;
				dTop   = 0;
				dIndex = 0;
				winMode = MODE_DISPLAY;

				updateWindow[SEARCH] 	= true;
				updateWindow[DISPLAY]	= true;
				updateWindow[ZOOM]		= true;
				updateWindow[PLAYLIST]	= true;

				break;/*}}}*/

			case ADD_VALUE:
			{/*{{{*/
				Form forma;

				if (forma.capture_value() == true) {
					dBase[mode].add_value(forma.get_value());
					dBase[mode].ordenate();
				}

				draw_windows();
				updateWindow[LCD]		= true;
				updateWindow[SEARCH] 	= true;
				updateWindow[DISPLAY]	= true;
				updateWindow[PLAYLIST] 	= true;
				updateWindow[COMPUTER] 	= true;
				updateWindow[DIGITS]	= true;
				updateWindow[ZOOM]		= true;	

				break;
			}/*}}}*/

			case DELETE_VALUE:
			{/*{{{*/
				int difference = displayTable[dIndex] - &(dBase[mode].base[0]);

				dBase[mode].delete_value(difference);


				llenado_displayTable(displayTable, dBase[mode].base, nRows[mode], keyword, &dRows);

				updateWindow[LCD]		= true;
				updateWindow[SEARCH] 	= true;
				updateWindow[DISPLAY]	= true;
				updateWindow[COMPUTER] 	= true;
				updateWindow[DIGITS]	= true;
				updateWindow[ZOOM]		= true;	

				break;
			}/*}}}*/

			case EDIT_VALUE:
			{/*{{{*/
				Form forma;

				int difference = displayTable[dIndex] - &(dBase[mode].base[0]);

				if (forma.capture_value(dBase[mode].base[difference]) == true) {
					dBase[mode].edit_value(difference, forma.get_value());
					dBase[mode].ordenate();
				}

				llenado_displayTable(displayTable, dBase[mode].base, nRows[mode], keyword, &dRows);

				draw_windows();
				updateWindow[LCD]		= true;
				updateWindow[SEARCH] 	= true;
				updateWindow[DISPLAY]	= true;
				updateWindow[PLAYLIST] 	= true;
				updateWindow[COMPUTER] 	= true;
				updateWindow[DIGITS]	= true;
				updateWindow[ZOOM]		= true;	

				break;
			}/*}}}*/

			case EDIT_ORCHESTRATION :/*{{{*/
				// ida
				buffer = displayTable[ dIndex ];
				// keyboard.set_program( *buffer );
				orquestacion.show( buffer );
				update_popups(); // se decide poner aquí para no refrescar varias veces
				orquestacion.capture_key();

				// retorno
				update_popups();
				draw_windows();
				updateWindow[LCD]		= true;
				updateWindow[SEARCH] 	= true;
				updateWindow[DISPLAY]	= true;
				updateWindow[PLAYLIST] 	= true;
				updateWindow[COMPUTER] 	= true;
				updateWindow[DIGITS]	= true;
				updateWindow[ZOOM]		= true;	
				break;/*}}}*/

			case TOGGLE_MIDI_STATE:/*{{{*/
				keyboard.toggle_MIDI_state();
				updateWindow[ MIDI_STATE ] = true;
				break;/*}}}*/

			case EXPORTATE:/*{{{*/
				dBase[COMBINATIONS].escribir( config_directory + "/combinations-2.csv" );

				*keyword = '\0';

				llenado_displayTable(displayTable, dBase[mode].base,  nRows[mode],  keyword, &dRows);

				charIndex = 0;
				dTop   = 0;
				dIndex = 0;
				winMode = MODE_DISPLAY;

				updateWindow[SEARCH] 	= true;
				updateWindow[DISPLAY]	= true;
				updateWindow[ZOOM]		= true;
				updateWindow[PLAYLIST]	= true;

				break;

			default:
				break;/*}}}*/
		}

		// Printing{{{
		if (updateWindow[DISPLAY] == true)
			print_displayTable(	displayWindow, 	displayTable,
								dTop, 	dRows, 	dIndex, winMode);

		if (updateWindow[PLAYLIST] == true)
			print_playlist(	playlistWindow, playlistTable,
							plTop, 	plRows, plIndexA, plIndexB, winMode);

		if (updateWindow[SEARCH] == true)
			print_search(searchWindow, keyword);

		if (updateWindow[LCD] == true)
			print_lcd( lcdWindow, buffer );

		if ( updateWindow[ MIDI_STATE ] )
			print_MIDI_state( MIDI_state_window, keyboard.get_MIDI_state() );

		if (updateWindow[ZOOM] == true) {
			if (winMode == MODE_PLAYLIST)
				print_zoom(zoomWindow, &playlistTable[plIndexB]);
			else if ( keyword[0] != '/' and dIndex >= 0 )
				print_zoom(zoomWindow, displayTable[dIndex]);
			else {
				wclear(zoomWindow);
				wrefresh(zoomWindow);
			}/*}}}*/
		}

		command = get_command(caracter = getch(), mode, winMode, keyword, charIndex, dIndex);

	} while (command != EXIT);

	endwin();

	delete [] displayTable;
	buffer = NULL;

	return EXIT_SUCCESS;

}
