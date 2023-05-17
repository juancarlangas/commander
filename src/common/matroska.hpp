#ifndef MATROSKA_HPP
#define MATROSKA_HPP

#include "common/common.hpp"

enum matroska/*{{{*/
	{	NEXT = 0,
		BEGIN,
		SET_MODE,
		SELECT,
		INTRO,
		CHANGE_WINDOW,
		MOVE_INDEX,
		DRAG_UP,
		DRAG_DOWN,
		PREV_VARIATION, NEXT_VARIATION,
		SET_VARIATION,
		CHANGE_HOTKEYS_BEHAVIOR,
		CHANGE_VARIATION,
		ADD_TO_PLAYLIST,
		DEL_FROM_PLAYLIST,
		COPY_ORCHESTRATION,
		PASTE_ORCHESTRATION,
		ESCAPE,
		DEL,
		READ_CHAR,
		SAVE_PLAYLIST,
		LOAD_PLAYLIST,
		CLEAR_PLAYLIST,
		FAVOURITE,
		RELOAD_CONNECTIONS,
		ADD_VALUE,
		EDIT_VALUE,
		EDIT_ORCHESTRATION,
		DELETE_VALUE,
		EXPORTATE,
		SELECT_PART,
		TOGGLE_MIDI_STATE,
		EXIT,
		EXPORTATE_AND_QUIT };/*}}}*/

enum matroska get_command(	const int digit,
							const short mode, short windowMode,
							char cadena[], short int ci,
							int dIndex ) noexcept;

#endif
