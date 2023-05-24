#include "main.hpp"

int32_t main()
{
	// Graphics{{{
	y = init_ncurses();
	draw_windows();
	short int updateWindow[8] = {0};/*}}}*/

	// Folders{{{
	const char *home_directory_c;
	if ( ( home_directory_c = getenv("HOME") ) == NULL )
		home_directory_c = getpwuid( getuid() )->pw_dir;
	const std::string home_directory { home_directory_c };

   	char config_directory_c[50];
   	sprintf( config_directory_c, "%s/.config/commander", home_directory_c );
	std::string generic_path;
    const std::string config_directory{ config_directory_c };/*}}}*/

	// Data{{{
	Programming programming {config_directory + "/combinations.json"};
	Catalog *dBase = new Catalog [] {{config_directory + "/catalog.json"}};
	Catalog& catalog {dBase[COMBINATIONS]};

	std::int32_t dbRows[1] { dBase[COMBINATIONS].get_activeRows() };
	std::int32_t& n_performances {dbRows[0]};
/*}}}*/

	// Variables{{{
	Performance **displayTable = new Performance *[n_performances](); // arreglo de apuntadores
	Performance* orch_clipboard_ptr, *performance_buffer; // apuntadores simple

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
	Keyboard x50;
	x50.set_name("X50");

	orquestacion.link_combinations ( &programming );
	orquestacion.link_MIDI_device( &x50 );/*}}}*/

	// Engine{{{
	do {
		for ( i = LCD; i <= ZOOM; i++ )
			updateWindow[i] = false;

		switch (command) {
			case BEGIN:/*{{{*/
				llenado_displayTable(displayTable, catalog.performances, n_performances, keyword, &n_matches);

				if (n_performances > 0) // permitimos 0 lineas
					performance_buffer = &catalog.performances.front();

				x50.set_performance_buffer( *performance_buffer );

				playlist->cargar( "/home/juancarlangas/.config/commander/Playlists/default.csv" );

				updateWindow[LCD]		= true;
				updateWindow[SEARCH] 	= true;
				updateWindow[DISPLAY]	= true;
				updateWindow[PLAYLIST] 	= true;
				updateWindow[COMPUTER] 	= true;
				updateWindow[DIGITS]	= true;
				updateWindow[ZOOM]		= true;
				updateWindow[MIDI_STATE] = true;

				break;/*}}}*/

			case SET_MODE:/*{{{*/
				for (k = 0; k <= LONG_STRING - 1; k++)
					keyword[k] = '\0';

				if (mode == COMBINATOR)
					mode = SEQUENCER;
				else
					mode = COMBINATOR;

				llenado_displayTable(
						displayTable, dBase[mode].performances, dbRows[mode], keyword, &n_matches);

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

				llenado_displayTable(
						displayTable, dBase[mode].performances, dbRows[mode], keyword, &n_matches);

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

				// si es acento o tilde, quedamos a la espera del siguiente
				if (keyword[charIndex - 1] != -61) {
					charIndex = no_accent( keyword, keyword ); //clean

					// Garantizamos que la última letra válida añadida sea minúscula
					// keyword[ charIndex - 1 ] = std::tolower( keyword[ charIndex - 1 ] );

					llenado_displayTable(	displayTable, dBase[mode].performances,
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

				llenado_displayTable(
						displayTable, dBase[mode].performances, dbRows[mode], keyword, &n_matches);
				
				dTop = 0;
				dIndex = 0;

				updateWindow[0] = 1;
				updateWindow[1] = 1;
				updateWindow[2] = 1;
				updateWindow[ZOOM] = true;

				if (winMode == MODE_PLAYLIST)
					winMode = MODE_DISPLAY;

				break;/*}}}*/

			case SET_SCENE: {/*{{{*/
				// la conversión de 1 a 9 es -48, pero por indice de arreglo restamos uno más
				int32_t funcion_a_variacion = caracter - KEY_F0;
				x50.set_scene( funcion_a_variacion );
				if ( x50.is_connected() )
					x50.dump_scene();
				break;
			}/*}}}*/

			case TO_PREV_SCENE:/*{{{*/
				x50.to_prev_scene();
				if ( x50.is_connected() )
					x50.dump_scene();
				break;/*}}}*/

			case TO_NEXT_SCENE :/*{{{*/
				x50.to_next_scene();
				if ( x50.is_connected() )
					x50.dump_scene();
				break;/*}}}*/

			case INTRO:/*{{{*/
				switch ( winMode ) {
					case MODE_DISPLAY:
						performance_buffer = displayTable[ dIndex ];
						break;
					case MODE_PLAYLIST:
						performance_buffer = playlist->get_pointer( pl_index );
						//avance carro
						if ( plIndexB < playlist->get_n_pistas() - 1 ) {
							plIndexB++;
							pl_index = plIndexB;
						}
						updateWindow[PLAYLIST] = true;
						updateWindow[ZOOM] = true;

						break;
				}

				if ( x50.is_connected() )
					x50.set_performance_buffer( *performance_buffer );
				else
					// solo actualizamos el buffer para poder trabajar online
					x50.set_performance_buffer( *performance_buffer );

				updateWindow[LCD] = true;

				break;/*}}}*/

			case FAVOURITE: {/*{{{*/
				int32_t caracter_a_numero = caracter == 48 ? 9 : ( caracter - 49 );
				performance_buffer = dBase[ COMBINATIONS ].get_favourite_row( caracter_a_numero );

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
					catalog.add_value(forma.get_value());
					catalog.ordenate();
					performance_buffer = &catalog.performances[0]; // Actualizamos después del cambio
				}

				dbRows[ COMBINATIONS ] = dBase[ COMBINATIONS ].get_activeRows();
				llenado_displayTable(
						displayTable, catalog.performances, dbRows[mode], keyword, &n_matches );

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

			case DELETE_VALUE:/*{{{*/
			{
				// obtenemos el índice real en dBase al cual displayTable[dIndex] apunta
				int real_index = displayTable[dIndex] - &(dBase[mode].performances[0]);

				dBase[mode].delete_value( real_index );
				performance_buffer = &catalog.performances[0]; // Actualizamos después del cambio


				llenado_displayTable(
						displayTable, dBase[mode].performances, dbRows[mode], keyword, &n_matches );

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

				// dIndex represents absolute iterator over displayTable, but NOT OVER catalog, so we whould
				// get the exact catalog index calculating the difference
				int difference = displayTable[dIndex] - &(catalog.performances[0]);

				if (forma.capture_value(dBase[mode].performances[difference]) == true) {
					dBase[mode].edit_value(difference, forma.get_value());
					dBase[mode].ordenate();
				}

				llenado_displayTable(displayTable, dBase[mode].performances, dbRows[mode], keyword, &n_matches);

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
				displayTable[ dIndex ]->n_scenes = orch_clipboard_ptr->n_scenes;
				displayTable[ dIndex]->default_scene = orch_clipboard_ptr->default_scene;
				for ( i = 0; i < orch_clipboard_ptr->n_scenes; ++i ) {
					displayTable[ dIndex ]->scenes[ i ].label =
						orch_clipboard_ptr->scenes[ i ].label;
					for ( j = 0; j < NUMBER_OF_PARTS; ++j ) {
						displayTable[ dIndex ]->scenes[ i ].tracks[ j ].state =
							orch_clipboard_ptr->scenes[ i ].tracks[ j ].state;
						displayTable[ dIndex ]->scenes[ i ].tracks[ j ].volume =
							orch_clipboard_ptr->scenes[ i ].tracks[ j ].volume;
						displayTable[ dIndex ]->scenes[ i ].tracks[ j ].lower_key =
							orch_clipboard_ptr->scenes[ i ].tracks[ j ].lower_key;
						displayTable[ dIndex ]->scenes[ i ].tracks[ j ].upper_key =
							orch_clipboard_ptr->scenes[ i ].tracks[ j ].upper_key;
						displayTable[ dIndex ]->scenes[ i ].tracks[ j ].transposition =
							orch_clipboard_ptr->scenes[ i ].tracks[ j ].transposition;
					}
				}
				break;/*}}}*/

			case EXPORTATE:/*{{{*/
				dBase[COMBINATIONS].save_to_json( config_directory + "/catalog.json" );
				//programming.( config_directory + "/programming.json" );

				*keyword = '\0';

				llenado_displayTable(
						displayTable, dBase[mode].performances,dbRows[mode],  keyword, &n_matches);

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
				dBase[COMBINATIONS].save_to_json( config_directory + "/catalog.json" );
				//programming.escribir( config_directory + "/programming.json" );

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
							"/home/juancarlangas/.commander/Playlists/" + cpp_file_name + ".csv" );
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
						displayTable, dBase[mode].performances, dbRows[mode], keyword, &n_matches);

				charIndex = 0;
				dTop   = 0;
				dIndex = 0;
				winMode = MODE_DISPLAY;

				updateWindow[SEARCH] 	= true;
				updateWindow[DISPLAY]	= true;
				updateWindow[ZOOM]		= true;
				updateWindow[PLAYLIST]	= true;

				break;/*}}}*/

			case ADD_TO_PLAYLIST:/*{{{*/
				playlist->agregar( displayTable[ dIndex ] );

				if ( playlist->get_n_pistas() > plTop + playlistShowResults )
					++plTop;

				playlist->guardar( home_directory + "/.commander/Playlists/default.csv" );

				updateWindow[PLAYLIST] = true;

				break;/*}}}*/

			case DEL_FROM_PLAYLIST:/*{{{*/
				playlist->eliminar( pl_index );
				playlist->guardar( home_directory + "/.commander/Playlists/default.csv" );

				if ( pl_index == playlist->get_n_pistas() ) {//fin de lista
					decrease_index( &plTop, &pl_index );
					plIndexB = pl_index; // Guardamos compatibilidad
				}

				if ( playlist->get_n_pistas() == 0 ) { //cambio
					winMode = MODE_DISPLAY;
					updateWindow[DISPLAY] = true;
				}
				updateWindow[LCD] 	   = true;
				updateWindow[PLAYLIST] = true;
				updateWindow[ZOOM]	= true;

				break;/*}}}*/

			case CLEAR_PLAYLIST:/*{{{*/
				// save_playlist(playlistTable, plRows, "default");

				for (k = 0; k <= LONG_STRING - 1; k++)
					keyword[k] = '\0';

				llenado_displayTable(
						displayTable, dBase[mode].performances, dbRows[mode],  keyword, &n_matches);

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
						"/home/juancarlangas/.commander/Playlists/" + cpp_file_name + ".csv" );
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
						displayTable, dBase[mode].performances, dbRows[mode],  keyword, &n_matches);

				charIndex = 0;
				dTop   = 0;
				dIndex = 0;
				winMode = MODE_DISPLAY;

				updateWindow[SEARCH] 	= true;
				updateWindow[DISPLAY]	= true;
				updateWindow[ZOOM]		= true;
				updateWindow[PLAYLIST]	= true;

				break;/*}}}*/

			case TOGGLE_MIDI_STATE:/*{{{*/
				x50.toggle_MIDI_state();
				updateWindow[ MIDI_STATE ] = true;
				break;/*}}}*/
		}

		// Printing{{{
		if (updateWindow[DISPLAY] == true)
			print_displayTable(	displayWindow, 	displayTable,
								dTop, 	n_matches, 	dIndex, winMode);

		if (updateWindow[PLAYLIST] == true) {
			//print_playlist(	playlistWindow, playlistTable,
			//				plTop, 	plRows, pl_index, plIndexB, winMode);
			print_playlist(	playlistWindow, plTop, pl_index, *playlist, winMode);
		}

		if (updateWindow[SEARCH] == true)
			print_search(searchWindow, keyword);

		if (updateWindow[LCD] == true)
			print_lcd( lcdWindow, performance_buffer );

		if ( updateWindow[ MIDI_STATE ] )
			print_MIDI_state( MIDI_state_window, x50.get_MIDI_state() );

		if (updateWindow[ZOOM] == true) {
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
