#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#include "midi/keyboards.hpp"

#include "data/databases.hpp"
#include "graphics/form.hpp"
#include "graphics/channelForm.hpp"
#include "common/common.hpp"
#include "common/string.hpp"
#include "main.hpp"
#include "graphics/windows.hpp"
#include "graphics/windows/zoom.hpp"

enum matroska get_command(int, short int, short int, Variation, char [], short int, int dIndex);

int main()
{
	//ncurses
	y = init_ncurses();
	draw_windows();
	short int updateWindow[7] = {0};

	//********************************** file system *********************************//
	//home directory
	const char *homedir;
	if ((homedir = getenv("HOME")) == NULL)
		homedir = getpwuid(getuid())->pw_dir;

   	char directory[50];
   	sprintf(directory, "%s/.commander", homedir);

   	//********************************* Databases ************************************//
	Database dBase[2];	// C++ Object (1 for combs, 1 for seqs)
	int nRows[2];

		nRows[COMBINATIONS] = dBase[COMBINATIONS].get_activeRows(directory, "Combinations");
		dBase[0].load(directory, "Combinations");

		nRows[SEQUENCES] 	= dBase[SEQUENCES].get_activeRows(directory, "Sequences");
		dBase[1].load(directory, "Sequences");

	//************************************* tables ***********************************//
	System *displayTable[1000], playlistTable[1000], *buffer;
	int favourite[10];

	int 	dRows, 		dTop  = 0, 	dIndex   = 0,
			plRows = 0, plTop = 0, 	plIndexA = 0, plIndexB = 0;
	char 	keyword[LONG_STRING] = "\0";
	int caracter;
	short int charIndex = 0;

	int i;
	short int k;

	//************************************** system ************************************//
	short int 	mode = COMBINATOR,
			winMode = MODE_DISPLAY;
	bool	 	oxygenBridge = CONNECTED;
	enum matroska command = BEGIN;
	Variation var = VAR2;
	short int part;
	bool nuevo = TRUE;

	//************************************** keyboards **********************************//
	Keyboard master, oxygen61;

	oxygen61.set_name("VI61");
		bool bridge;
		sprintf(directory, "%s/bridge_state.dat", homedir);

	master.set_name("X50");

	short channel;
		FILE *channelFile = fopen("/home/juancarlangas/.commander/midiChannel.dat", "r");
			fscanf(channelFile, "%hd", &channel);
		fclose(channelFile);

	ChannelForm channelForm;

	//*************************************** engine ***********************************//
	do {
		for (i = LCD; i <= ZOOM; i++)
			updateWindow[i] = false;

		switch (command) {

			/////////////////////////// BEGIN /////////////////////////////////
			case BEGIN:

				llenado_displayTable(displayTable, dBase[COMBINATIONS].base, nRows[COMBINATOR], 
										keyword, &dRows);
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

				break;

			/////////////////////////// SET_MODE ///////////////////////////
			case SET_MODE:
				for (k = 0; k <= LONG_STRING - 1; k++)
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
				nuevo = TRUE;

				updateWindow[SEARCH] 	= true;
				updateWindow[DISPLAY]	= true;
				updateWindow[COMPUTER] 	= true;
				updateWindow[DIGITS]	= true;
				updateWindow[ZOOM]		= true;

				break;

			/////////////////////////// ESCAPE ///////////////////////////
			case ESCAPE:
				for (k = 0; k <= LONG_STRING - 1; k++)
					keyword[k] = '\0';

				llenado_displayTable(displayTable, dBase[mode].base, nRows[mode], keyword, &dRows);

				charIndex = 0;
				dTop = 0;
				dIndex = 0;
				nuevo = TRUE;

				if (winMode == MODE_PLAYLIST) {
					winMode = MODE_DISPLAY;
					updateWindow[PLAYLIST] = true;
				}
				updateWindow[LCD] 	  = true;
				updateWindow[SEARCH]  = true;
				updateWindow[DISPLAY] = true;
				updateWindow[ZOOM]	  = true;

				break;

			/////////////////////////// CHANGE_VARIATION ///////////////////////////
			case CHANGE_VARIATION:

				if (var == VAR2)
					var = VAR1;
				else
					var = VAR2;

				nuevo = TRUE;

				updateWindow[COMPUTER] = true;

				break;

			/////////////////////////// SELECT_PART ///////////////////////////
			case SELECT:	
					if (master.part == 1)
						master.select_part(2);
					else
						master.select_part(1);
				break;

			/////////////////////////// INTRO ///////////////////////////
			case INTRO:
				switch (winMode) {
					case MODE_DISPLAY:
						communicator(master, buffer = displayTable[dIndex], var, master.midiChannel);
						break;
					case MODE_PLAYLIST:
						communicator(master, buffer = &playlistTable[plIndexA], var, master.midiChannel);
						//avance carro
						if (plIndexB < plRows - 1) {
							plIndexB++;
							plIndexA = plIndexB;
						}
						updateWindow[PLAYLIST] = true;
						updateWindow[ZOOM] = true;
						break;
				}
				//if (master.part == 2);
				//	master.select_part(1);

				//ESTE CODIGO LO QUEREMOS TRABAJAR ////////////////////
				//master.select_part(1);
				//nuevo = false;

				updateWindow[LCD] = true;

				break;

			/////////////////////////// CHANGE_WINDOW ///////////////////////////
			case CHANGE_WINDOW:

				if (winMode == MODE_DISPLAY && plRows > 0) {
					winMode = MODE_PLAYLIST;
					if (plIndexA < plTop && plIndexB < plTop) {// <-- Que no se quede volando
						plIndexA = plTop;
						plIndexB = plTop;
					}
				}
				else if (winMode == MODE_PLAYLIST)
					winMode = MODE_DISPLAY;

				nuevo = TRUE;


				updateWindow[DISPLAY] 	= true;
				updateWindow[PLAYLIST] 	= true;
				updateWindow[ZOOM]		= true;

				break;

			/////////////////////////// MOVE_INDEX ///////////////////////////
			case MOVE_INDEX:

				if (caracter == KEY_UP || caracter == KEY_LEFT)
					switch (winMode) {
						case MODE_DISPLAY:
							decrease_index(&dTop, dRows, &dIndex, winMode);
							updateWindow[DISPLAY] = true;
							break;
						case MODE_PLAYLIST:
							decrease_index(&plTop, plRows, &plIndexB, winMode);
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
				nuevo = TRUE;

				updateWindow[DIGITS] = true;
				updateWindow[ZOOM]	= true;

				break;

			/////////////////////////// ADD ///////////////////////////
			case ADD:
				playlistTable[(plRows)++] = *displayTable[dIndex];
				if (plRows > plTop + playlistShowResults)
					(plTop)++;

				save_playlist(playlistTable, (const int)plRows, "default");

				nuevo = TRUE;

				updateWindow[PLAYLIST] = true;

				break;

			/////////////////////////// SUPR ///////////////////////////
			case SUPR:
				for (i = plIndexA; i <= plRows - 1; i++)  //succión
					playlistTable[i] = playlistTable[i + 1];
				if (plIndexA == plRows - 1) { //fin de lista
					decrease_index(&plTop, plRows, &plIndexA, winMode);
					(plIndexB = plIndexA);
				}
				(plRows)--;

				save_playlist(playlistTable, (const int)plRows, "default");

				nuevo = TRUE;

				if (plRows == 0) { //cambio
					winMode = MODE_DISPLAY;
					updateWindow[DISPLAY] = true;
				}
				updateWindow[LCD] 	   = true;
				updateWindow[PLAYLIST] = true;
				updateWindow[ZOOM]	= true;

				break;

			/////////////////////////// DEL ///////////////////////////
			case DEL:

				charIndex--;
				keyword[charIndex] = '\0';

				llenado_displayTable(displayTable, dBase[mode].base, nRows[mode], keyword, &dRows);
				
				dTop = 0;
				dIndex = 0;
				nuevo = TRUE;

				updateWindow[0] = 1;
				updateWindow[1] = 1;
				updateWindow[2] = 1;
				updateWindow[ZOOM] = true;

				if (winMode == MODE_PLAYLIST)
					winMode = MODE_DISPLAY;

				break;

			/////////////////////////// DRAG ///////////////////////////
			case DRAG_UP: case DRAG_DOWN:

				if (winMode == MODE_PLAYLIST && caracter == 566 && plIndexB > 0) {
					korg_drag(playlistTable, plRows, plIndexB, plIndexB, caracter);
					decrease_index(&plTop, plRows, &plIndexB, winMode);
					plIndexA = plIndexB;
					save_playlist(playlistTable, (const int)plRows, "default");
					updateWindow[PLAYLIST] = true;
				}

				else if (winMode == MODE_PLAYLIST && caracter == 525 && plIndexB < plRows - 1) {
					korg_drag(playlistTable, plRows, plIndexB, plIndexB, caracter);
					increase_index(&plTop, plRows, &plIndexB, winMode);
					plIndexA = plIndexB;
					save_playlist(playlistTable, (const int)plRows, "default");
					updateWindow[PLAYLIST] = true;
				}
				
				break;

			/////////////////////////// READ_CHAR ///////////////////////////
			case READ_CHAR:

				//agregate
				keyword[charIndex++] = caracter;
				keyword[charIndex] = '\0';

				if (keyword[charIndex - 1] != -61) { //skip alter-character
					charIndex = no_accent(keyword, keyword); //clean

					llenado_displayTable(displayTable, dBase[mode].base, nRows[mode], keyword, &dRows);

					if (dRows == 0)
						dTop = dIndex = -1;
					else
						dTop = dIndex = 0;

					nuevo = TRUE;

					if (winMode == MODE_PLAYLIST) {
						winMode = MODE_DISPLAY;
						updateWindow[PLAYLIST] = true;
					}

					if (keyword[0] != '/' || charIndex == 1)
						updateWindow[ZOOM] = true;

					updateWindow[LCD] 	  = true;
					updateWindow[SEARCH]  = true;
					updateWindow[DISPLAY] = true;

				}

				break;

			/////////////////////////// SAVE_PLAYLIST ///////////////////////////
			case SAVE_PLAYLIST:
			{
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
					save_playlist(playlistTable, (const int)plRows, file_name);

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

				break;
			}

			/////////////////////////// LOAD_PLAYLIST ///////////////////////////
			case LOAD_PLAYLIST:
			{
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
					plRows = load_playlist(playlistTable, file_name);

				noecho();
				wclear(ventana[INPUT_BOX]);
				curs_set(false);
				raw();

				hide_panel(panel[DIALOG_WINDOW]);
				hide_panel(panel[INPUT_BOX]);
				update_panels();
				doupdate();

				save_playlist(playlistTable, (const int)plRows, "default");

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
			}
			/////////////////////////// CLEAR PLAYLIST ///////////////////////////
			case CLEAR_PLAYLIST:

				plRows = 0;
				save_playlist(playlistTable, (const int)plRows, "default");

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

				break;

			/////////////////// ADD_VALUE //////////////////////
			case ADD_VALUE:
			{
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
			}

			////////////////////// DELETE_VALUE ///////////////////////
			case DELETE_VALUE:
			{
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
			}

			//////////////////////// EDIT_VALUE /////////////////////////77
			case EDIT_VALUE:
			{
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
			}

			///////////////////////// EXPORTATE /////////////////////
			case EXPORTATE:

				if (mode == COMBINATIONS)
					dBase[COMBINATIONS].exportate("Combinations");
				else
					dBase[SEQUENCES].exportate("Sequences");

				sprintf(keyword, "");

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

		}
		///////////////////////////////////////	printing ///////////////////////////////////////

		if (updateWindow[DISPLAY] == true)
			print_displayTable(	displayWindow, 	displayTable,
								dTop, 	dRows, 	dIndex, winMode);

		if (updateWindow[PLAYLIST] == true)
			print_playlist(	playlistWindow, playlistTable,
							plTop, 	plRows, plIndexA, plIndexB, mode, winMode);

		if (updateWindow[SEARCH] == true)
			print_search(searchWindow, keyword);

		if (updateWindow[LCD] == true)
			print_lcd(lcdWindow, buffer, var);

		if (updateWindow[COMPUTER] == true) {
			print_computer(computerWindow, oxygenBridge, mode, var);
			print_mode(computerWindow, mode);
		}

		if (updateWindow[ZOOM] == true)
			if (winMode == MODE_PLAYLIST)
				print_zoom(zoomWindow, &playlistTable[plIndexB]);
			else if (keyword[0] != '/' && dIndex >= 0)
				print_zoom(zoomWindow, displayTable[dIndex]);
			else {
				wclear(zoomWindow);
				wrefresh(zoomWindow);
			}

		command = get_command(caracter = getch(), mode, winMode, var, keyword, charIndex, dIndex);

	} while (command != EXIT);

	endwin();
	return 0;

}

//***********************************+ get_command *************************************
enum matroska get_command(	const int digit,
							const short mode, short windowMode, Variation variation, 
							char cadena[], short int ci,
							int dIndex												)
{
	enum matroska comando = NEXT;

	switch (digit) {

		case '\n': //ESCAPE SEQUENCE*
			if (strstr("/add", cadena) != NULL && strstr(cadena, "/add") != NULL)
				//if (cadena[0] == '\0'); // Se garantiza que no hay búsquedas
					comando = ADD_VALUE;

			else if (strstr("/seq", cadena) != NULL && strstr(cadena, "/seq") != NULL) {
				if (mode == COMBINATOR)
					comando = SET_MODE;
				else
					comando = ESCAPE;
			}
			else if (strstr("/cmb", cadena) != NULL && strstr(cadena, "/cmb") != NULL) { 
				if (mode == SEQUENCER)
					comando = SET_MODE;
				else
					comando = ESCAPE;
			}

			else if (strstr("/save", cadena) != NULL && strstr(cadena, "/save") != NULL)
				comando = SAVE_PLAYLIST;

			else if (strstr("/load", cadena) != NULL && strstr(cadena, "/load") != NULL)
				comando = LOAD_PLAYLIST;

			else if (strstr("/clear", cadena) != NULL && strstr(cadena, "/clear") != NULL)
				comando = CLEAR_PLAYLIST;

			else if (strstr("/export", cadena) != NULL && strstr(cadena, "/export") != NULL)
				comando = EXPORTATE;

			else
				comando = INTRO;

            //else if (strstr("/channel", cadena) != NULL && strstr(cadena, "/channel") != NULL)
				//comando = SET_MIDI_CHANNEL;
			break;

		case '+':
			if (windowMode == MODE_DISPLAY && dIndex >= 0)
				comando = ADD;
			break;

		case 27:
			comando = ESCAPE;
			break;

		case KEY_F(1):
			comando = CHANGE_VARIATION;
			break;

		case ' ':
			comando = SELECT;
			break;

		case '\t':
			comando = CHANGE_WINDOW;
			break;

		case KEY_UP: case KEY_DOWN:
			comando = MOVE_INDEX;
			break;

		case KEY_LEFT: case KEY_RIGHT:
			comando = SELECT_PART;
			break;

		case 566: case 525:
			if (digit == 566)
				comando = DRAG_UP;
			else
				comando = DRAG_DOWN;
			break;

		case KEY_DC:
			if (windowMode == MODE_PLAYLIST)
				comando = SUPR;
			break;

		case KEY_BACKSPACE:
			if (ci > 0)
				comando = DEL;
			break;

		case 18:
			comando = RELOAD_CONNECTIONS;
			break;

		case KEY_F(2):
			if (windowMode == MODE_DISPLAY)
				comando = EDIT_VALUE;
			break;

		case 519: //CTL+SUPR
			if (windowMode == MODE_DISPLAY)
				comando = DELETE_VALUE;
			break;

		case 3:
			comando = EXIT;
			break;

		default:
			if (48 <= digit && digit <= 122 || digit == '/')
				comando = READ_CHAR;
			else if (48 <= digit && digit <= 57)
				comando = FAVOURITE;
            break;
	}

	return comando;
}
