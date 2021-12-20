#ifndef WINDOWS_H

#define WINDOWS_H

#include "../common/common.hpp"
#include "elements/windows/popups/orchestra_elements/text_popups/field_popup.hpp"
#include "orchestra.hpp"

//C functions
extern	WINDOW 	*searchBox,		*searchWindow,
				*lcdBox,		*lcdWindow,
				*zoomBox,		*zoomWindow,
				*computerBox,	*computerWindow,
				*displayBox,	*displayWindow,
				*playlistBox,	*playlistWindow,
				*digitsBox,		*digitsWindow,
				*MIDI_state_window;

extern Orchestra orquestacion;

extern WINDOW 	*ventana[2];
extern PANEL 	*panel[2];

extern	short int x, y;
extern	short displayShowResults, playlistShowResults;

#endif
