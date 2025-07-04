#include <stdio.h>/*{{{*/
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <filesystem>
#include <panel.h>/*}}}*/

#include "common/common.hpp"/*{{{*/
#include "common/matroska.hpp"
#include "ui/Orchestra.hpp"
#include "midi/Keyboard.hpp"
#include "ui/form.hpp"
#include "ui/ncurses.hpp"
#include "ui/screen.hpp"
#include "common/common.hpp"
#include "common/string.hpp"
#include "common/matroska.hpp"
#include "data/tables.hpp"
#include "data/Catalog.hpp"
#include "data/Playlist.hpp"
#include "ui/printing.hpp"
#include "utils/environment.hpp"
/*}}}*/

enum HotKeysMode {
	Variations,
	Favourites
};

std::int32_t main() {
	// Graphics{{{
	set_windows();
	draw_windows();
	std::array<std::int16_t,8> update_window {0};/*}}}*/

	// Folders{{{
	const std::filesystem::path home_dir {get_home_dir()};
	const std::filesystem::path config_dir {
		home_dir/".config/commander"};/*}}}*/

	// Data{{{
	Catalog* dBase = new Catalog [] {{config_dir/"catalog.json"}};
	Catalog& catalog {dBase[COMBINATIONS]};
	catalog.set_sfz_folder(home_dir/".sounds"/"sfz"/"commander");

	std::int32_t n_performances {catalog.get_activeRows()};
/*}}}*/

	// Variables{{{
	Performance** displayTable = new Performance* [1000](); // arreglo de apuntadores
	Performance *orch_clipboard_ptr, *performance_buffer; // apuntadores simple

	Playlist *playlist = new Playlist( &catalog );

	int32_t n_matches;
	int32_t dIndex { 0 }; // Absolute selected index of the whole displayTable
	int32_t dTop { 0 }; // Absolute on-window-top index of the whole displayTable

	int32_t plTop = { 0 };
	int32_t pl_index = { 0 };
	int32_t plIndexB = { 0 };

	char 	keyword[LONG_STRING] { "\0" };
	int32_t	caracter;
	int16_t charIndex { 0 };

	int32_t i, j;
	int16_t k;

	short int	mode = COMBINATOR,
				winMode = MODE_DISPLAY;
	enum matroska command = BEGIN;/*}}}*/

	// Keyboard{{{
	Keyboard x50 {config_dir/"combinations.json"};

	orquestacion.link_MIDI_device( &x50 );/*}}}*/

	// Engine{{{
	do {
		for ( i = LCD; i <= ZOOM; i++ )
			update_window[i] = false;

		switch (command) {
			case BEGIN:/*{{{*/
				llenado_displayTable(
					displayTable, catalog.performances, n_performances,
					keyword, &n_matches);

				if (n_performances > 0) // permitimos 0 lineas
					performance_buffer = &catalog.performances.front();

				x50.set_performance_buffer( *performance_buffer );

				playlist->cargar(config_dir/"Playlists"/"default.csv");

				update_window[LCD]		= true;
				update_window[SEARCH] 	= true;
				update_window[DISPLAY]	= true;
				update_window[PLAYLIST] 	= true;
				update_window[COMPUTER] 	= true;
				update_window[DIGITS]	= true;
				update_window[ZOOM]		= true;
				update_window[MIDI_STATE] = true;

				break;/*}}}*/

			case TOGGLE_WORK_MODE:/*{{{*/
				for (k = 0; k <= LONG_STRING - 1; k++)
					keyword[k] = '\0';

				if (mode == COMBINATOR)
					mode = SEQUENCER;
				else
					mode = COMBINATOR;

				llenado_displayTable(displayTable,
						dBase[mode].performances,
						n_performances, keyword, &n_matches);

				charIndex = 0;
				dTop   = 0;
				dIndex = 0;
				winMode = MODE_DISPLAY;

				update_window[SEARCH] 	= true;
				update_window[DISPLAY]	= true;
				update_window[COMPUTER] 	= true;
				update_window[DIGITS]	= true;
				update_window[ZOOM]		= true;

				break;/*}}}*/

			case RESET_QUERY:/*{{{*/
				for (k = 0; k <= LONG_STRING - 1; k++)
					keyword[k] = '\0';

				llenado_displayTable(displayTable, dBase[mode].performances,
						n_performances, keyword, &n_matches);

				charIndex = 0;
				dTop = 0;
				dIndex = 0;

				if (winMode == MODE_PLAYLIST) {
					winMode = MODE_DISPLAY;
					update_window[PLAYLIST] = true;
				}
				update_window[LCD] 	  = true;
				update_window[SEARCH]  = true;
				update_window[DISPLAY] = true;
				update_window[ZOOM]	  = true;

				break;/*}}}*/

			case INCREASE_QUERY:/*{{{*/

				// agregamos 1 letra al keyword
				keyword[ charIndex++ ] = caracter;
				keyword[ charIndex ] = '\0';

				// si es acento o tilde, quedamos a la espera del siguiente
				if (keyword[charIndex - 1] != -61) {
					charIndex = no_accent( keyword, keyword ); //clean

					llenado_displayTable(	displayTable, dBase[mode].performances,
											n_performances, keyword, &n_matches	);

					if (winMode == MODE_PLAYLIST) {
						winMode = MODE_DISPLAY;
						update_window[PLAYLIST] = true;
					}

					if (keyword[0] != ':' || charIndex == 1)
						update_window[ZOOM] = true;


					update_window[LCD] 	  = true;
					update_window[SEARCH]  = true;
					update_window[DISPLAY] = true;

				}
				break;/*}}}*/

			case DECREASE_QUERY:/*{{{*/

				charIndex--;
				keyword[charIndex] = '\0';

				llenado_displayTable(displayTable, dBase[mode].performances,
						n_performances, keyword, &n_matches);
				
				dTop = 0;
				dIndex = 0;

				update_window[0] = 1;
				update_window[1] = 1;
				update_window[2] = 1;
				update_window[ZOOM] = true;

				if (winMode == MODE_PLAYLIST)
					winMode = MODE_DISPLAY;

				break;/*}}}*/

			case DIAL_FAVOURITE: {/*{{{*/
				performance_buffer =
					dBase[COMBINATIONS].
					get_favourite_row(caracter - KEY_F0 - 1);

				if (x50.is_connected())
					x50.dump_performance(*performance_buffer);

				update_window[LCD] 	  = true;

				/* Toda la siguiente parte reinicializa displayTable y coloca el índice
				 * en el favorito, pero esta es una característica poco conveniente.
				 *
				for (k = 0; k <= LONG_STRING - 1; k++)
					keyword[k] = '\0';

				llenado_displayTable(displayTable, dBase[mode].base, dbRows[mode], keyword, &n_matches);

				charIndex = 0;
				dTop = 0;
				dIndex = buffer - dBase[COMBINATIONS].base; // Maravilloso!!! Aritmética de pointer.

				if (winMode == MODE_PLAYLIST) {
					winMode = MODE_DISPLAY;
					update_window[PLAYLIST] = true;
				}
				update_window[SEARCH]  = true;
				update_window[DISPLAY] = true;
				update_window[ZOOM]	  = true;
				*/

				break;
			}/*}}}*/

			case TO_PREV_SCENE:/*{{{*/
				x50.to_prev_scene();
				//if (x50.is_connected())
					//x50.dump_scene();
				break;/*}}}*/

			case TO_NEXT_SCENE :/*{{{*/
				x50.to_next_scene();
				//if ( x50.is_connected() )
					//x50.dump_scene();
				break;/*}}}*/

			case INTRO:/*{{{*/
				switch ( winMode ) {
					case MODE_DISPLAY:
						performance_buffer = displayTable[ dIndex ];
						break;
					case MODE_PLAYLIST:
						performance_buffer =
							playlist->get_pointer( pl_index );
						//avance carro
						if ( plIndexB < playlist->get_n_pistas() - 1 ) {
							plIndexB++;
							pl_index = plIndexB;
						}
						update_window[PLAYLIST] = true;
						update_window[ZOOM] = true;

						break;
				}

				if ( x50.is_connected() )
					x50.dump_performance( *performance_buffer );
				else
					//solo actualizamos buffer para poder trabajar online
					x50.set_performance_buffer( *performance_buffer );

				x50.write_sfz_file(
						catalog.get_sfz_folder(), "commander.sfz",
						performance_buffer->sfz_filename);

				update_window[LCD] = true;

				break;/*}}}*/

			case FAVOURITE: {/*{{{*/
				int32_t caracter_a_numero =
					caracter == 48 ? 9 : ( caracter - 49 );
				performance_buffer =
					dBase[COMBINATIONS].
					get_favourite_row(caracter_a_numero);

				if (x50.is_connected())
					x50.dump_performance(*performance_buffer);

				update_window[LCD] 	  = true;

				/* Toda la siguiente parte reinicializa displayTable y coloca el índice
				 * en el favorito, pero esta es una característica poco conveniente.
				 *
				for (k = 0; k <= LONG_STRING - 1; k++)
					keyword[k] = '\0';

				llenado_displayTable(displayTable, dBase[mode].base, dbRows[mode], keyword, &n_matches);

				charIndex = 0;
				dTop = 0;
				dIndex = buffer - dBase[COMBINATIONS].base; // Maravilloso!!! Aritmética de pointer.

				if (winMode == MODE_PLAYLIST) {
					winMode = MODE_DISPLAY;
					update_window[PLAYLIST] = true;
				}
				update_window[SEARCH]  = true;
				update_window[DISPLAY] = true;
				update_window[ZOOM]	  = true;
				*/

				break;
			}/*}}}*/

			case CHANGE_WINDOW:/*{{{*/

				if (winMode == MODE_DISPLAY && playlist->get_n_pistas() > 0) {
					winMode = MODE_PLAYLIST;
					if (pl_index < plTop && plIndexB < plTop) {// <-- Que no se quede volando
						pl_index = plTop;
						plIndexB = plTop;
					}
				}
				else if (winMode == MODE_PLAYLIST)
					winMode = MODE_DISPLAY;

				update_window[DISPLAY] 	= true;
				update_window[PLAYLIST] 	= true;
				update_window[ZOOM]		= true;

				break;/*}}}*/

			case MOVE_INDEX:/*{{{*/
				if (caracter == KEY_UP || caracter == KEY_LEFT)
					switch (winMode) {
						case MODE_DISPLAY:
							decrease_index( &dTop, &dIndex );
							update_window[DISPLAY] = true;
							break;
						case MODE_PLAYLIST:
							decrease_index(&plTop, &plIndexB );
							pl_index = plIndexB;
							update_window[PLAYLIST] = true;
							break;
					}
				else if (caracter == KEY_DOWN || caracter == KEY_RIGHT)
					switch (winMode) {
						case MODE_DISPLAY:
							increase_index(&dTop, n_matches, &dIndex, winMode);
							update_window[DISPLAY] = true;
							break;
						case MODE_PLAYLIST:
							increase_index(&plTop, playlist->get_n_pistas(), &plIndexB, winMode);
							pl_index = plIndexB;
							update_window[PLAYLIST] = true;
							break;
					}

				update_window[DIGITS] = true;
				update_window[ZOOM]	= true;

				break;/*}}}*/

			case ADD_VALUE:/*{{{*/
			{
				Form forma;

				if (forma.capture_value() == true) {
					catalog.add_value(forma.get_value());
					catalog.ordenate();
					performance_buffer = &catalog.performances[0]; // Actualizamos después del cambio
				}

				n_performances = dBase[ COMBINATIONS ].get_activeRows();
				llenado_displayTable(
						displayTable, catalog.performances, n_performances, keyword, &n_matches );

				draw_windows();
				update_window[LCD]		= true;
				update_window[SEARCH] 	= true;
				update_window[DISPLAY]	= true;
				update_window[PLAYLIST] 	= true;
				update_window[COMPUTER] 	= true;
				update_window[DIGITS]	= true;
				update_window[ZOOM]		= true;	


				break;
			}/*}}}*/

			case DELETE_VALUE:/*{{{*/
			{
				// obtenemos el índice real en dBase al cual displayTable[dIndex] apunta
				std::size_t real_index {static_cast<std::size_t>(displayTable[dIndex] - &catalog.performances[0])};

				catalog.delete_value(real_index);
				n_performances = catalog.get_activeRows();
				performance_buffer = &catalog.performances[0]; // Actualizamos después del cambio


				llenado_displayTable(
						displayTable, dBase[mode].performances, n_performances, keyword, &n_matches );

				update_window[LCD]		= true;
				update_window[SEARCH] 	= true;
				update_window[DISPLAY]	= true;
				update_window[COMPUTER] 	= true;
				update_window[DIGITS]	= true;
				update_window[ZOOM]		= true;	

				n_performances = catalog.get_activeRows();

				// We procurate the indexed element dont get in the air like the Coyote when deleting
				// the last element
				if (dIndex > n_performances - 1)
					--dIndex;

				break;
			}/*}}}*/

			case EDIT_VALUE:/*{{{*/
			{
				Form forma;

				// dIndex represents absolute iterator over displayTable, but NOT OVER catalog, so we whould
				// get the exact catalog index calculating the difference
				int difference = displayTable[dIndex] - &(catalog.performances[0]);

				if (forma.capture_value(dBase[mode].performances[difference]) == true) {
					dBase[mode].edit_value(difference, forma.get_value());
					dBase[mode].ordenate();
				}

				llenado_displayTable(displayTable, dBase[mode].performances, n_performances, keyword, &n_matches);

				draw_windows();
				update_window[LCD]		= true;
				update_window[SEARCH] 	= true;
				update_window[DISPLAY]	= true;
				update_window[PLAYLIST] 	= true;
				update_window[COMPUTER] 	= true;
				update_window[DIGITS]	= true;
				update_window[ZOOM]		= true;	

				break;
			}/*}}}*/

			case EDIT_ORCHESTRATION :/*{{{*/
				if ( winMode == MODE_DISPLAY ) {
					performance_buffer = displayTable[dIndex];

					if (performance_buffer->n_scenes == 0)
						orquestacion.add_empty_scene(performance_buffer);
					orquestacion.reset_variation();
					orquestacion.show( performance_buffer );
					update_popups(); // se decide poner aquí para no refrescar varias veces
					orquestacion.capture_key();

					// retorno
					update_popups();
					draw_windows();

					update_window[LCD]		= true;
					update_window[SEARCH] 	= true;
					update_window[DISPLAY]	= true;
					update_window[PLAYLIST] 	= true;
					update_window[COMPUTER] 	= true;
					update_window[DIGITS]	= true;
					update_window[ZOOM]		= true;	
				}

				break;/*}}}*/

			case COPY_ORCHESTRATION :/*{{{*/
				orch_clipboard_ptr = displayTable[dIndex];
				break;/*}}}*/

			case PASTE_ORCHESTRATION :/*{{{*/
				// Lets redim the scenes vector
				displayTable[dIndex]->scenes.clear();
				displayTable[dIndex]->scenes.resize(orch_clipboard_ptr->n_scenes);

				// Lets copy the values
				displayTable[dIndex]->n_scenes = orch_clipboard_ptr->n_scenes;
				displayTable[dIndex]->default_scene = orch_clipboard_ptr->default_scene;

				for (i = 0; i < orch_clipboard_ptr->n_scenes; ++i) {
					displayTable[dIndex]->scenes[i].label =
						orch_clipboard_ptr->scenes[i].label;
					for (j = 0; j < static_cast<int>(NUMBER_OF_PARTS); ++j) {
						displayTable[dIndex]->scenes[i].strips[j].state =
							orch_clipboard_ptr->scenes[i].strips[j].state;
						displayTable[dIndex]->scenes[i].strips[j].volume =
							orch_clipboard_ptr->scenes[i].strips[j].volume;
						displayTable[dIndex]->scenes[i].strips[j].lower_key =
							orch_clipboard_ptr->scenes[i].strips[j].lower_key;
						displayTable[dIndex]->scenes[i].strips[j].upper_key =
							orch_clipboard_ptr->scenes[i].strips[j].upper_key;
						displayTable[dIndex]->scenes[i].strips[j].transposition =
							orch_clipboard_ptr->scenes[i].strips[j].transposition;
					}
				}
				break;/*}}}*/

			case EXPORTATE:/*{{{*/
				catalog.save_to_json(config_dir/"catalog.json" );
				x50.save_combs_to_json(config_dir/"combinations.json");

				*keyword = '\0';

				llenado_displayTable(
						displayTable, dBase[mode].performances, n_performances,  keyword, &n_matches);

				charIndex = 0;
				dTop   = 0;
				dIndex = 0;
				winMode = MODE_DISPLAY;

				update_window[SEARCH] 	= true;
				update_window[DISPLAY]	= true;
				update_window[ZOOM]		= true;
				update_window[PLAYLIST]	= true;

				break;/*}}}*/

			case EXPORTATE_AND_QUIT :/*{{{*/
				// Esta madrola hace lo básico del EXPORTATE y diréctamente modifica
				// el command para que el while saque a la chingada el programa
				catalog.save_to_json(config_dir/"catalog.json");
				x50.save_combs_to_json(config_dir/"combinations.json");

				command = EXIT;

				break;

			default:
				break;/*}}}*/

			case LOAD_PLAYLIST:/*{{{*/

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
					std::string cpp_file_name{ file_name };
					playlist->cargar(
							"/home/juancarlangas/.config/commander/Playlists/" + cpp_file_name + ".csv" );
				}
				noecho();
				wclear(ventana[INPUT_BOX]);
				curs_set(false);
				raw();

				hide_panel(panel[DIALOG_WINDOW]);
				hide_panel(panel[INPUT_BOX]);
				update_panels();
				doupdate();

				// save_playlist(playlistTable, plRows, "default");
				playlist->guardar( "/home/juancarlangas/.commander/Playlists/default.csv" );

				for (k = 0; k <= LONG_STRING - 1; k++)
					keyword[k] = '\0';

				llenado_displayTable(
						displayTable, dBase[mode].performances,  n_performances, keyword, &n_matches);

				charIndex = 0;
				dTop   = 0;
				dIndex = 0;
				winMode = MODE_DISPLAY;

				update_window[SEARCH] 	= true;
				update_window[DISPLAY]	= true;
				update_window[ZOOM]		= true;
				update_window[PLAYLIST]	= true;

				break;/*}}}*/

			case ADD_TO_PLAYLIST:/*{{{*/
				playlist->agregar( displayTable[ dIndex ] );

				if ( playlist->get_n_pistas() > plTop + playlistShowResults )
					++plTop;

				playlist->guardar(home_dir/".config"/"commander"/"Playlists"/"default.csv");

				update_window[PLAYLIST] = true;

				break;/*}}}*/

			case DEL_FROM_PLAYLIST:/*{{{*/
				playlist->eliminar( pl_index );
				playlist->guardar(home_dir/".config"/"commander"/"Playlists"/"default.csv");
				if ( pl_index == playlist->get_n_pistas() ) {//fin de lista
					decrease_index( &plTop, &pl_index );
					plIndexB = pl_index; // Guardamos compatibilidad
				}

				if ( playlist->get_n_pistas() == 0 ) { //cambio
					winMode = MODE_DISPLAY;
					update_window[DISPLAY] = true;
				}
				update_window[LCD] 	   = true;
				update_window[PLAYLIST] = true;
				update_window[ZOOM]	= true;

				break;/*}}}*/

			case CLEAR_PLAYLIST:/*{{{*/
				// save_playlist(playlistTable, plRows, "default");

				for (k = 0; k <= LONG_STRING - 1; k++)
					keyword[k] = '\0';

				llenado_displayTable(
						displayTable, dBase[mode].performances, n_performances, keyword, &n_matches);

				if (winMode == MODE_PLAYLIST)
					winMode = MODE_DISPLAY;

				charIndex = 0;
				dTop   = 0;
				dIndex = 0;
				winMode = MODE_DISPLAY;

				update_window[SEARCH] 	= true;
				update_window[DISPLAY]	= true;
				update_window[ZOOM]		= true;
				update_window[PLAYLIST]	= true;

				break;/*}}}*/

			case DRAG_UP: case DRAG_DOWN:/*{{{*/
				break;/*}}}*/

			case SAVE_PLAYLIST:/*{{{*/

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
				std::string cpp_file_name{ file_name };
				playlist->guardar(
						"/home/juancarlangas/.config/commander/Playlists/" + cpp_file_name + ".csv" );
				}

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

				llenado_displayTable(
						displayTable, dBase[mode].performances, n_performances,  keyword, &n_matches);

				charIndex = 0;
				dTop   = 0;
				dIndex = 0;
				winMode = MODE_DISPLAY;

				update_window[SEARCH] 	= true;
				update_window[DISPLAY]	= true;
				update_window[ZOOM]		= true;
				update_window[PLAYLIST]	= true;

				break;/*}}}*/

			case TOGGLE_MIDI_STATE:/*{{{*/
				x50.toggle_MIDI_state();
				update_window[ MIDI_STATE ] = true;
				break;/*}}}*/
		}

		// Printing{{{
		if (update_window[DISPLAY] == true)
			print_displayTable(	displayWindow, 	displayTable,
								dTop, 	n_matches, 	dIndex, winMode);

		if (update_window[PLAYLIST] == true) {
			//print_playlist(	playlistWindow, playlistTable,
			//				plTop, 	plRows, pl_index, plIndexB, winMode);
			print_playlist(	playlistWindow, plTop, pl_index, *playlist, winMode);
		}

		if (update_window[SEARCH] == true)
			print_search(searchWindow, keyword);

		if (update_window[LCD] == true)
			print_lcd( lcdWindow, performance_buffer );

		if ( update_window[ MIDI_STATE ] )
			print_MIDI_state( MIDI_state_window, x50.get_MIDI_state() );

		if (update_window[ZOOM] == true) {
			if (winMode == MODE_PLAYLIST)
				print_zoom(zoomWindow, playlist->get_pointer( pl_index ) );
			else if ( keyword[0] != ':' and dIndex >= 0 )
				print_zoom(zoomWindow, displayTable[dIndex]);
			else {
				wclear(zoomWindow);
				wrefresh(zoomWindow);
			}
		}/*}}}*/

			/* ENVÍO MIDI:{{{
			 * se realiza en esta parte ya que visualmente deseamos que primero haga los cambios
			 * en la pantalla y después se tome el tiempo de terminar con el proceso MIDI. Hacer esto
			 * garantiza una percepción fluida del software.
			 *
			 * Por lo pronto reservamos esta funcionalidad para el comando FAVOURITE*/
			if ( command == FAVOURITE and x50.is_connected() )
				x50.set_performance_buffer( *performance_buffer );/*}}}*/

		if ( command != EXIT ) // Que no eche a perder el :w
			command = get_command(caracter = getch(), mode, winMode, keyword, charIndex, dIndex);

	} while ( command != EXIT );/*}}}*/

	// Ending{{{
	if ( x50.get_MIDI_state() == Switch::ON )
		x50.disconnect();

	endwin();

	delete playlist;

	delete [] displayTable;
	performance_buffer = NULL;

	delete [] dBase;

	return EXIT_SUCCESS;/*}}}*/
}
