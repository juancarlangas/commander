#include "common/matroska.hpp"
#include "common.hpp"
#include <curses.h>

enum matroska get_command(const int digit, const short mode,
		short windowMode, std::string_view _Query, short int ci, int dIndex) noexcept
{
	enum matroska comando = NEXT;

	switch (digit) {

		// ENTER{{{
		case '\n': //RESET_QUERY SEQUENCE*
			if (_Query == ":add")
					comando = ADD_VALUE;

			else if (_Query == ":del") {
				if (windowMode == MODE_DISPLAY)
					comando = DELETE_VALUE;
			}

			else if (_Query == ":seq") {
				if (mode == COMBINATOR)
					comando = TOGGLE_WORK_MODE;
				else
					comando = RESET_QUERY;
			}

			else if (_Query == ":cmb") {
				if (mode == SEQUENCER)
					comando = TOGGLE_WORK_MODE;
				else
					comando = RESET_QUERY;
			}

			else if (_Query == ":wpl")
				comando = SAVE_PLAYLIST;

			else if (_Query == ":lpl")
				comando = LOAD_PLAYLIST;

			else if (_Query == ":cpl")
				comando = CLEAR_PLAYLIST;

			else if (_Query == ":w" or _Query == ":wa")
				comando = EXPORTATE;

			else if (_Query == ":q" or _Query == ":qa")
				comando = EXIT;

			else if (_Query == ":wq" or _Query == ":wqa")
				comando = EXPORTATE_AND_QUIT;

			else
				comando = INTRO;

			break;/*}}}*/

		case 11: // <C-K>
			comando = TOGGLE_MIDI_STATE;
			break;
		case 20: // <C-K>
			if (windowMode == MODE_DISPLAY)
				 comando = EDIT_VALUE;
			break;
		case 15: // <C-O>
			if (windowMode == MODE_DISPLAY)
				comando = EDIT_ORCHESTRATION;
			break;
		case 3: // <C-C>
			if (windowMode == MODE_DISPLAY)
				comando = COPY_ORCHESTRATION;
			break;
		case 22: // <C-P>
			if (windowMode == MODE_DISPLAY)
				 comando = PASTE_ORCHESTRATION;
			 break;
		case '+':
			 if (windowMode == MODE_DISPLAY && dIndex >= 0)
				 comando = ADD_TO_PLAYLIST;
			 break;
		case '-':
			 if (windowMode == MODE_PLAYLIST)
				 comando = DEL_FROM_PLAYLIST;
			 break;
		case 27:
			 comando = RESET_QUERY;
			 break;
		case '\t':
			 comando = CHANGE_WINDOW;
			 break;
		case KEY_UP: case KEY_DOWN:
			 comando = MOVE_INDEX;
			 break;
		case KEY_LEFT:
			 comando = TO_PREV_SCENE;
			 break;
		case KEY_RIGHT:
			 comando = TO_NEXT_SCENE;
			 break;
		case 566:
			 comando = DRAG_UP;
			 break;
		case 525:
			 comando = DRAG_DOWN;
			 break;
		case 520:
			if (windowMode == MODE_DISPLAY)
				comando = DELETE_VALUE;
			else
				comando = DEL_FROM_PLAYLIST;
			break;

		case KEY_BACKSPACE: case 8:
			if (ci > 0) comando = DECREASE_QUERY;
			break;

		case 17:
			comando = EXIT;
			break;

		default:
			if ( KEY_F(1) <= digit and digit <= KEY_F(12) )
				comando = DIAL_FAVOURITE;
			else if ( ( 58 <= digit and digit <= 122 ) || digit == '/' )
				comando = INCREASE_QUERY;
            break;
	}

	return comando;
}
