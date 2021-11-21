#include "common/matroska.hpp"
#include <string.h>

enum matroska get_command(	const int digit,
							const short mode, short windowMode, Variation variation, 
							char cadena[], short int ci,
							int dIndex )
{
	enum matroska comando = NEXT;

	switch (digit) {

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

			else if (strstr(":w", cadena) != NULL && strstr(cadena, ":save") != NULL)
				comando = SAVE_PLAYLIST;

			else if (strstr(":load", cadena) != NULL && strstr(cadena, ":load") != NULL)
				comando = LOAD_PLAYLIST;

			else if (strstr(":clear", cadena) != NULL && strstr(cadena, ":clear") != NULL)
				comando = CLEAR_PLAYLIST;

			else if (strstr(":export", cadena) != NULL && strstr(cadena, ":export") != NULL)
				comando = EXPORTATE;

			else if ( strcmp( ":q", cadena ) == 0 )
				comando = EXIT;

			else
				comando = INTRO;

			break;

		case 18 : // CTRL-R
			if ( windowMode == MODE_DISPLAY and dIndex >= 0 )
				comando = CARGAR_ESPECIFICO;
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

		// Flechas
		case KEY_UP: case KEY_DOWN:
			comando = MOVE_INDEX;
			break;

		case KEY_LEFT:	comando = PREV_VARIATION; break;
		case KEY_RIGHT:	comando = NEXT_VARIATION; break;

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
