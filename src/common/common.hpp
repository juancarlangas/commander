#ifndef COMMON_H
#define COMMON_H

#include <ncurses.h>
#include <panel.h>

/* Length (TOTAL_ROWS 268) */
#define COMB_ROWS 	205
#define SEQ_ROWS 	78
#define LONG_STRING	41

/* MODE */
#define COMBINATION 0
#define COMBINATOR	0
#define COMBINATIONS 0
#define SEQUENCER	1
#define SEQUENCES	1

/* status */
#define CONTINUE	0
#define CHANGE_MODE	1
#define SEND_DATA	2
#define PLAY_SEQUENCE	3

enum matroska {	NEXT = 0,
				BEGIN,
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
				DELETE_VALUE,
				EXPORTATE,
				SELECT_PART,
				EXIT				};

/* WINDOWS */
#define LCD			0
#define SEARCH		1
#define DISPLAY		2
#define PLAYLIST	3
#define COMPUTER 	4
#define DIGITS		5
#define ZOOM		6

/* WINDOWS GENERAL */
#define DIALOG_WINDOW	0
#define INPUT_BOX		1

/* WINMODE */
#define MODE_DISPLAY	1
#define MODE_PLAYLIST	2

// MIDI Connections
#define DISCONNECTED 	0
#define CONNECTED		1

//Commander 1.0
#define QUERYBOX	 1
#define PLAYLISTBOX	 2

typedef enum {VAR1 = 1, VAR2} Variation;

class Configuration {
public:
	Configuration();
	void set_variation(Variation);
private:
	Variation variazzione;
};

#endif
