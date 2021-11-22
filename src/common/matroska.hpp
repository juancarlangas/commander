#ifndef MATROSKA_HPP
#define MATROSKA_HPP

#include "common/common.hpp"

enum matroska {	NEXT = 0,
				BEGIN,
				CARGAR_ESPECIFICO,
				SET_MODE,
				SELECT,
				INTRO,
				CHANGE_WINDOW,
				MOVE_INDEX,
				DRAG_UP,
				DRAG_DOWN,
				PREV_VARIATION, NEXT_VARIATION,
				CHANGE_VARIATION,
				ADD,
				SUPR,
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
				CLOSE_ORCHESTRA,
				DELETE_VALUE,
				EXPORTATE,
				SELECT_PART,
				EXIT				};

enum matroska get_command(	const int digit,
							const short mode, short windowMode,
							char cadena[], short int ci,
							int dIndex ) noexcept;

#endif
