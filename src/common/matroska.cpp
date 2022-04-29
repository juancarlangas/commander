#include "common/matroska.hpp"
#include "common.hpp"
#include <string.h>

enum matroska get_command(	const int digit, const short mode, short windowMode, 
							char cadena[], short int ci, int dIndex ) noexcept
{
	enum matroska comando = NEXT;

	switch (digit) {

		// ENTER{{{
		case '\n': //ESCAPE SEQUENCE*
			if (strstr(":add", cadena) != NULL && strstr(cadena, ":add") != NULL)
				//if (cadena[0] == '\0'); // Se garantiza que no hay búsquedas
					comando = ADD_VALUE;

			else if (strstr(":seq", cadena) != NULL && strstr(cadena, ":seq") != NULL) {
				if (mode == COMBINATOR)
					comando = SET_MODE;
				else
					comando = ESCAPE;
			}
			else if (strstr(":cmb", cadena) != NULL && strstr(cadena, ":cmb") != NULL) { 
				if (mode == SEQUENCER)
					comando = SET_MODE;
				else
					comando = ESCAPE;
			}

			else if (strstr(":save", cadena) != NULL && strstr(cadena, ":wpl") != NULL)
				comando = SAVE_PLAYLIST;

			else if (strstr(":load", cadena) != NULL && strstr(cadena, ":load") != NULL)
				comando = LOAD_PLAYLIST;

			else if (strstr(":clear", cadena) != NULL && strstr(cadena, ":clear") != NULL)
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
		case 520: if (windowMode == MODE_DISPLAY) comando = DELETE_VALUE; break; //<C-Supr>
		case 3: if ( windowMode == MODE_DISPLAY ) comando = COPY_ORCHESTRATION; break; // <C-C>
		case 22: if ( windowMode == MODE_DISPLAY ) comando = PASTE_ORCHESTRATION; break; // <C-P>
		case '+': if (windowMode == MODE_DISPLAY && dIndex >= 0) comando = ADD; break;
		case 27: comando = ESCAPE; break;
		case '\t': comando = CHANGE_WINDOW; break;
		case KEY_UP: case KEY_DOWN: comando = MOVE_INDEX; break;
		case KEY_LEFT:	comando = PREV_VARIATION; break;
		case KEY_RIGHT:	comando = NEXT_VARIATION; break;
		case 566: comando = DRAG_UP; break;
		case 525: comando = DRAG_DOWN; break;
		case KEY_DC: if (windowMode == MODE_PLAYLIST) comando = SUPR; break;
		case KEY_BACKSPACE: if (ci > 0) comando = DEL; break; // BACKSPACE
		case 17: comando = EXIT; break;

		default:
			if ( KEY_F(1) <= digit and digit <= KEY_F(10) )
				comando = SET_VARIATION;
			else if (  48 <= digit and digit <= 57 )
				comando = FAVOURITE;
			else if ( ( 58 <= digit and digit <= 122 ) || digit == '/' )
				comando = READ_CHAR;
            break;
	}

	return comando;
}
