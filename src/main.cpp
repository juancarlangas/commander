#include "main.hpp"
#include "common/common.hpp"
#include "common/matroska.hpp"
#include "graphics/printing.hpp"

int32_t main()
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
    const std::string config_directory{ directory };

	Combinations combinaciones { config_directory + "/combinaciones.csv" };

	Database dBase[1]{ { config_directory + "/catalogo.csv", &combinaciones } };

	int dbRows[1];
	dbRows[COMBINATIONS] = dBase[COMBINATIONS].get_activeRows();
/*}}}*/

	// Tables{{{
	System **displayTable = new System *[ dbRows[COMBINATIONS] ](); // arreglo de apuntadores
	System *playlistTable = new System [ dbRows[COMBINATIONS] ](); // arreglo de copias
	System *buffer, *orch_clipboard_ptr; // apuntadores simple

	Playlist *playlist = new Playlist( "/home/juancarlangas/.commander/Playlists/default.cpl",
										dBase );

	int32_t n_matches;
	int32_t dIndex { 0 }; // Absolute selected index of the whole displayTable
	int32_t dTop { 0 }; // Absolute on-window-top index of the whole displayTable

	int32_t plRows { 0 };
	int32_t plTop = { 0 };
	int32_t pl_index = { 0 };
	int32_t plIndexB = { 0 };

	char 	keyword[LONG_STRING] { "\0" };
	int32_t	caracter;
	int16_t charIndex { 0 };

	int32_t i, j;
	int16_t k;/*}}}*/

	// System{{{
	short int	mode = COMBINATOR,
				winMode = MODE_DISPLAY;
	enum matroska command = BEGIN;/*}}}*/

	// Keyboards{{{
	Keyboard x50;
	x50.set_name("X50");

	orquestacion.link_combinations ( &combinaciones );
	orquestacion.link_MIDI_device( &x50 );/*}}}*/

	// Engine{{{
	do {
		for ( i = LCD; i <= ZOOM; i++ )
			updateWindow[i] = false;

		switch (command) {
			case BEGIN:/*{{{*/
				llenado_displayTable( displayTable, dBase[COMBINATIONS].base, dbRows[COMBINATOR],
						keyword, &n_matches );
				plRows = load_playlist( playlistTable, "default" );

				if (dbRows[COMBINATOR] > 0) // permitimos 0 lineas
					buffer = dBase[COMBINATIONS].base;

				x50.set_buffer( *buffer );

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
				dBase[ COMBINATIONS ].cargar_especifico( config_directory + "/catalogo.csv",
						buffer - dBase[ COMBINATIONS ].base );
				buffer = displayTable[ dIndex ];
				// x50.select_page( TIMBRE );
				x50.set_buffer( *buffer );
				x50.dump_variation();
				break;/*}}}*/

			case SET_MODE:/*{{{*/
				for (k = 0; k <= LONG_STRING - 1; k++)
					keyword[k] = '\0';

				if (mode == COMBINATOR)
					mode = SEQUENCER;
				else
					mode = COMBINATOR;

				llenado_displayTable(displayTable, dBase[mode].base,  dbRows[mode],  keyword, &n_matches);

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

			case ESCAPE:/*{{{*/
				for (k = 0; k <= LONG_STRING - 1; k++)
					keyword[k] = '\0';

				llenado_displayTable(displayTable, dBase[mode].base, dbRows[mode], keyword, &n_matches);

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

			case READ_CHAR:/*{{{*/

				// agregamos 1 letra al keyword
				keyword[ charIndex++ ] = caracter;
				keyword[ charIndex ] = '\0';

				if (keyword[charIndex - 1] != -61) {	// si es acento o tilde esperamos
														// la parte faltante
					charIndex = no_accent(keyword, keyword); //clean

					// Garantizamos que la última letra válida añadida sea minúscula
					// keyword[ charIndex - 1 ] = std::tolower( keyword[ charIndex - 1 ] );

					llenado_displayTable(	displayTable, dBase[mode].base,
											dbRows[mode], keyword, &n_matches	);

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

			case DEL:/*{{{*/

				charIndex--;
				keyword[charIndex] = '\0';

				llenado_displayTable(displayTable, dBase[mode].base, dbRows[mode], keyword, &n_matches);
				
				dTop = 0;
				dIndex = 0;

				updateWindow[0] = 1;
				updateWindow[1] = 1;
				updateWindow[2] = 1;
				updateWindow[ZOOM] = true;

				if (winMode == MODE_PLAYLIST)
					winMode = MODE_DISPLAY;

				break;/*}}}*/

			case SET_VARIATION: {/*{{{*/
				// la conversión de 1 a 9 es -48, pero por indice de arreglo restamos uno más
				int32_t funcion_a_variacion = caracter - KEY_F0;
				x50.set_variation( funcion_a_variacion );
				if ( x50.is_connected() )
					x50.dump_variation();
				break;
			}/*}}}*/

			case PREV_VARIATION:/*{{{*/
				x50.prev_variation();
				if ( x50.is_connected() )
					x50.dump_variation();
				break;/*}}}*/

			case NEXT_VARIATION :/*{{{*/
				x50.next_variation();
				if ( x50.is_connected() )
					x50.dump_variation();
				break;/*}}}*/
				
			case INTRO:/*{{{*/
				switch ( winMode ) {
					case MODE_DISPLAY:
						buffer = displayTable[ dIndex ];
						break;
					case MODE_PLAYLIST:
						buffer = &playlistTable[ pl_index ];
						//avance carro
						if ( plIndexB < plRows - 1 ) {
							plIndexB++;
							pl_index = plIndexB;
						}
						updateWindow[PLAYLIST] = true;
						updateWindow[ZOOM] = true;

						break;
				}

				if ( x50.is_connected() )
					x50.set_program( *buffer );
				else
					// solo actualizamos el buffer para poder trabajar online
					x50.set_buffer( *buffer );

				updateWindow[LCD] = true;

				break;/*}}}*/

			case FAVOURITE: {/*{{{*/
				int32_t caracter_a_numero = caracter == 48 ? 9 : ( caracter - 49 );
				buffer = dBase[ COMBINATIONS ].get_favourite_row( caracter_a_numero );

				updateWindow[LCD] 	  = true;

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
					updateWindow[PLAYLIST] = true;
				}
				updateWindow[SEARCH]  = true;
				updateWindow[DISPLAY] = true;
				updateWindow[ZOOM]	  = true;
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

				updateWindow[DISPLAY] 	= true;
				updateWindow[PLAYLIST] 	= true;
				updateWindow[ZOOM]		= true;

				break;/*}}}*/

			case MOVE_INDEX:/*{{{*/

				if ( caracter == KEY_UP || caracter == KEY_LEFT )
					switch ( winMode ) {
						case MODE_DISPLAY:
							decrease_index( &dTop, &dIndex );
							updateWindow[DISPLAY] = true;
							break;
						case MODE_PLAYLIST:
							decrease_index(&plTop, &plIndexB );
							pl_index = plIndexB;
							updateWindow[PLAYLIST] = true;
							break;
					}

				else if (caracter == KEY_DOWN || caracter == KEY_RIGHT)
					switch (winMode) {
						case MODE_DISPLAY:
							increase_index(&dTop, n_matches, &dIndex, winMode);
							updateWindow[DISPLAY] = true;
							break;
						case MODE_PLAYLIST:
							increase_index(&plTop, playlist->get_n_pistas(), &plIndexB, winMode);
							pl_index = plIndexB;
							updateWindow[PLAYLIST] = true;
							break;
					}

				updateWindow[DIGITS] = true;
				updateWindow[ZOOM]	= true;

				break;/*}}}*/

			case ADD_VALUE:/*{{{*/
			{
				Form forma;

				if (forma.capture_value() == true) {
					dBase[mode].add_value(forma.get_value());
					dBase[mode].ordenate();
				}

				dbRows[ COMBINATIONS ] = dBase[ COMBINATIONS ].get_activeRows();
				llenado_displayTable(
						displayTable, dBase[mode].base, dbRows[mode], keyword, &n_matches );

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

			case ADD_TO_PLAYLIST:/*{{{*/
				playlist->agregar( displayTable[ dIndex ] );

				if ( playlist->get_n_pistas() > plTop + playlistShowResults )
					--plTop;

				updateWindow[PLAYLIST] = true;

				break;/*}}}*/

			case DEL_FROM_PLAYLIST:/*{{{*/
				playlist->eliminar( pl_index );

				if ( pl_index == playlist->get_n_pistas() - 1 ) { //fin de lista
					decrease_index( &plTop, &pl_index );
					plRows = playlist->get_n_pistas();
				}

				if ( playlist->get_n_pistas() == 0 ) { //cambio
					winMode = MODE_DISPLAY;
					updateWindow[DISPLAY] = true;
				}
				updateWindow[LCD] 	   = true;
				updateWindow[PLAYLIST] = true;
				updateWindow[ZOOM]	= true;

				break;/*}}}*/

			case DRAG_UP: case DRAG_DOWN:/*{{{*/

				if ( winMode == MODE_PLAYLIST and caracter == 566 and plIndexB > 0) {
					korg_drag(playlistTable, plRows, plIndexB, plIndexB, caracter);
					decrease_index(&plTop, &plIndexB );
					pl_index = plIndexB;
					save_playlist(playlistTable, plRows, "default");
					updateWindow[PLAYLIST] = true;
				}

				else if (winMode == MODE_PLAYLIST && caracter == 525 && plIndexB < plRows - 1) {
					korg_drag(playlistTable, plRows, plIndexB, plIndexB, caracter);
					increase_index(&plTop, plRows, &plIndexB, winMode);
					pl_index = plIndexB;
					save_playlist(playlistTable, plRows, "default");
					updateWindow[PLAYLIST] = true;
				}
				
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

				llenado_displayTable(displayTable, dBase[mode].base,  dbRows[mode],  keyword, &n_matches);

				charIndex = 0;
				dTop   = 0;
				dIndex = 0;
				winMode = MODE_DISPLAY;

				updateWindow[SEARCH] 	= true;
				updateWindow[DISPLAY]	= true;
				updateWindow[ZOOM]		= true;
				updateWindow[PLAYLIST]	= true;

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

				llenado_displayTable(displayTable, dBase[mode].base,  dbRows[mode],  keyword, &n_matches);

				charIndex = 0;
				dTop   = 0;
				dIndex = 0;
				winMode = MODE_DISPLAY;

				updateWindow[SEARCH] 	= true;
				updateWindow[DISPLAY]	= true;
				updateWindow[ZOOM]		= true;
				updateWindow[PLAYLIST]	= true;

				break;/*}}}*/

			case CLEAR_PLAYLIST:/*{{{*/

				plRows = 0;
				save_playlist(playlistTable, plRows, "default");

				for (k = 0; k <= LONG_STRING - 1; k++)
					keyword[k] = '\0';

				llenado_displayTable(displayTable, dBase[mode].base,  dbRows[mode],  keyword, &n_matches);

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

			case DELETE_VALUE:/*{{{*/
			{
				// obtenemos el índice real en dBase al cual displayTable[dIndex] apunta
				int real_index = displayTable[dIndex] - &(dBase[mode].base[0]);

				dBase[mode].delete_value( real_index );


				llenado_displayTable(
						displayTable, dBase[mode].base, dbRows[mode], keyword, &n_matches );

				updateWindow[LCD]		= true;
				updateWindow[SEARCH] 	= true;
				updateWindow[DISPLAY]	= true;
				updateWindow[COMPUTER] 	= true;
				updateWindow[DIGITS]	= true;
				updateWindow[ZOOM]		= true;	

				dbRows[ COMBINATIONS ] = dBase[ COMBINATIONS ].get_activeRows();

				break;
			}/*}}}*/

			case EDIT_VALUE:/*{{{*/
			{
				Form forma;

				int difference = displayTable[dIndex] - &(dBase[mode].base[0]);

				if (forma.capture_value(dBase[mode].base[difference]) == true) {
					dBase[mode].edit_value(difference, forma.get_value());
					dBase[mode].ordenate();
				}

				llenado_displayTable(displayTable, dBase[mode].base, dbRows[mode], keyword, &n_matches);

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
				if ( winMode == MODE_DISPLAY ) {
					// ida
					/* Tal vez esto no sea útil
					if ( x50.get_MIDI_state() == Switch::ON )
					buffer = displayTable[ dIndex ];
					x50.set_program( *buffer );
					*/
					buffer = displayTable[ dIndex ];
					orquestacion.reset_variation();
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
				}

				break;/*}}}*/

			case COPY_ORCHESTRATION :/*{{{*/
				orch_clipboard_ptr = displayTable[ dIndex ];
				break;/*}}}*/

			case PASTE_ORCHESTRATION :/*{{{*/
				displayTable[ dIndex ]->n_variaciones = orch_clipboard_ptr->n_variaciones;
				displayTable[ dIndex]->variacion_inicial = orch_clipboard_ptr->variacion_inicial;
				for ( i = 0; i < orch_clipboard_ptr->n_variaciones; ++i ) {
					displayTable[ dIndex ]->variacion[ i ].etiqueta =
						orch_clipboard_ptr->variacion[ i ].etiqueta;
					for ( j = 0; j < NUMBER_OF_PARTS; ++j ) {
						displayTable[ dIndex ]->variacion[ i ].track[ j ].status =
							orch_clipboard_ptr->variacion[ i ].track[ j ].status;
						displayTable[ dIndex ]->variacion[ i ].track[ j ].volume =
							orch_clipboard_ptr->variacion[ i ].track[ j ].volume;
						displayTable[ dIndex ]->variacion[ i ].track[ j ].lower_key =
							orch_clipboard_ptr->variacion[ i ].track[ j ].lower_key;
						displayTable[ dIndex ]->variacion[ i ].track[ j ].upper_key =
							orch_clipboard_ptr->variacion[ i ].track[ j ].upper_key;
						displayTable[ dIndex ]->variacion[ i ].track[ j ].transposition =
							orch_clipboard_ptr->variacion[ i ].track[ j ].transposition;
					}
				}
				break;/*}}}*/

			case TOGGLE_MIDI_STATE:/*{{{*/
				x50.toggle_MIDI_state();
				updateWindow[ MIDI_STATE ] = true;
				break;/*}}}*/

			case EXPORTATE:/*{{{*/
				dBase[COMBINATIONS].escribir( config_directory + "/catalogo.csv" );
				combinaciones.escribir( config_directory + "/combinaciones.csv" );

				*keyword = '\0';

				llenado_displayTable(displayTable, dBase[mode].base,  dbRows[mode],  keyword, &n_matches);

				charIndex = 0;
				dTop   = 0;
				dIndex = 0;
				winMode = MODE_DISPLAY;

				updateWindow[SEARCH] 	= true;
				updateWindow[DISPLAY]	= true;
				updateWindow[ZOOM]		= true;
				updateWindow[PLAYLIST]	= true;

				break;/*}}}*/

			case EXPORTATE_AND_QUIT :/*{{{*/
				// Esta madrola hace lo básico del EXPORTATE y diréctamente modifica
				// el command para que el while saque a la chingada el programa
				dBase[COMBINATIONS].escribir( config_directory + "/catalogo.csv" );
				combinaciones.escribir( config_directory + "/combinaciones.csv" );

				command = EXIT;

				break;

			default:
				break;/*}}}*/
		}

		// Printing{{{
		if (updateWindow[DISPLAY] == true)
			print_displayTable(	displayWindow, 	displayTable,
								dTop, 	n_matches, 	dIndex, winMode);

		if (updateWindow[PLAYLIST] == true) {
			//print_playlist(	playlistWindow, playlistTable,
			//				plTop, 	plRows, pl_index, plIndexB, winMode);
			print_playlist(	playlistWindow, plTop, pl_index, plIndexB, *playlist, winMode);
		}

		if (updateWindow[SEARCH] == true)
			print_search(searchWindow, keyword);

		if (updateWindow[LCD] == true)
			print_lcd( lcdWindow, buffer );

		if ( updateWindow[ MIDI_STATE ] )
			print_MIDI_state( MIDI_state_window, x50.get_MIDI_state() );

		if (updateWindow[ZOOM] == true) {
			if (winMode == MODE_PLAYLIST)
				print_zoom(zoomWindow, &playlistTable[plIndexB]);
			else if ( keyword[0] != '/' and dIndex >= 0 )
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
				x50.set_program( *buffer );/*}}}*/

		if ( command != EXIT ) // Que no eche a perder el :wq
			command = get_command(caracter = getch(), mode, winMode, keyword, charIndex, dIndex);

	} while ( command != EXIT );/*}}}*/

	// Ending{{{
	if ( x50.get_MIDI_state() == Switch::ON )
		x50.disconnect();

	endwin();

	delete playlist;

	delete [] displayTable;
	buffer = NULL;

	return EXIT_SUCCESS;/*}}}*/
}
