#include "common/matroska.hpp"
#include "common.hpp"
#include <curses.h>
#include <string.h>

enum matroska get_command(	const int digit, const short mode, short windowMode, 
							char cadena[], short int ci, int dIndex ) noexcept
{
	enum matroska comando = NEXT;

	switch (digit) {

		// ENTER{{{
		case '\n': //RESET_QUERY SEQUENCE*
			if (strstr(":add", cadena) != NULL && strstr(cadena, ":add") != NULL)
				//if (cadena[0] == '\0'); // Se garantiza que no hay búsquedas
					comando = ADD_VALUE;

			else if (strcmp(":del", cadena) == 0) {
				//if (cadena[0] == '\0'); // Se garantiza que no hay búsquedas
				if (windowMode == MODE_DISPLAY)
					comando = DELETE_VALUE;
			}

			else if (strstr(":seq", cadena) != NULL && strstr(cadena, ":seq") != NULL) {
				if (mode == COMBINATOR)
					comando = TOGGLE_WORK_MODE;
				else
					comando = RESET_QUERY;
			}
			else if (strstr(":cmb", cadena) != NULL && strstr(cadena, ":cmb") != NULL) { 
				if (mode == SEQUENCER)
					comando = TOGGLE_WORK_MODE;
				else
					comando = RESET_QUERY;
			}

			else if ( !strcmp( cadena, ":wpl" ) )
				comando = SAVE_PLAYLIST;

			else if ( !strcmp( cadena, ":lpl" ) )
				comando = LOAD_PLAYLIST;

			else if ( !strcmp( cadena, ":cpl" ) )
				comando = CLEAR_PLAYLIST;

			else if (strstr(":w", cadena) != NULL && strstr(cadena, ":w") != NULL)
				comando = EXPORTATE;

			else if ( strcmp( ":q", cadena ) == 0 )
				comando = EXIT;

			else if ( strcmp( ":wq", cadena ) == 0 )
				comando = EXPORTATE_AND_QUIT;

			else
				comando = INTRO;

			break;/*}}}*/

		case 11: comando = TOGGLE_MIDI_STATE; break; //<C-K>
		case 20: if (windowMode == MODE_DISPLAY) comando = EDIT_VALUE; break; //<C-T>
		case 15: if ( windowMode == MODE_DISPLAY ) comando = EDIT_ORCHESTRATION; break; //<C-O>
		case 3: if ( windowMode == MODE_DISPLAY ) comando = COPY_ORCHESTRATION; break; // <C-C>
		case 22: if ( windowMode == MODE_DISPLAY ) comando = PASTE_ORCHESTRATION; break; // <C-P>
		case '+': if (windowMode == MODE_DISPLAY && dIndex >= 0) comando = ADD_TO_PLAYLIST; break;
		case '-': if (windowMode == MODE_PLAYLIST) comando = DEL_FROM_PLAYLIST; break;
		case 27: comando = RESET_QUERY; break;
		case '\t': comando = CHANGE_WINDOW; break;
		case KEY_UP: case KEY_DOWN: comando = MOVE_INDEX; break;
		case KEY_LEFT:	comando = TO_PREV_SCENE; break;
		case KEY_RIGHT:	comando = TO_NEXT_SCENE; break;
		case 566: comando = DRAG_UP; break;
		case 525: comando = DRAG_DOWN; break;
		case 520:
			if (windowMode == MODE_DISPLAY)
				comando = DELETE_VALUE;
			else
				comando = DEL_FROM_PLAYLIST;
			break;

		case KEY_BACKSPACE: case 8:
			if (ci > 0) comando = DECREASE_QUERY;
			break;

		case 17: comando = EXIT; break;

		default:
			if ( KEY_F(1) <= digit and digit <= KEY_F(12) )
				comando = DIAL_FAVOURITE;
			else if ( ( 58 <= digit and digit <= 122 ) || digit == '/' )
				comando = INCREASE_QUERY;
            break;
	}

	return comando;
}
