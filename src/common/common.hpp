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


/* WINDOWS */
#define LCD			0
#define SEARCH		1
#define DISPLAY		2
#define PLAYLIST	3
#define COMPUTER 	4
#define DIGITS		5
#define ZOOM		6
#define MIDI_STATE	7

/* WINDOWS GENERAL */
#define DIALOG_WINDOW	0
#define INPUT_BOX		1

/* WINMODE */
#define MODE_DISPLAY	1
#define MODE_PLAYLIST	2
#define MODE_ORCHESTRA	3

// MIDI Connections
#define DISCONNECTED 	0
#define CONNECTED		1

//Commander 1.0
#define QUERYBOX	 1
#define PLAYLISTBOX	 2

typedef enum {VAR1 = 1, VAR2} Variation;

#define MAX_VARIATIONS 10

class Configuration {
public:
	Configuration();
	void set_variation(Variation);
private:
	Variation variazzione;
};

#endif
