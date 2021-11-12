#ifndef WINDOWS_H
#define WINDOWS_H

#include "common.h"
#include "Windows/zoom.h"

//C functions
extern	WINDOW 	*searchBox,		*searchWindow,
				*lcdBox,		*lcdWindow,
				*zoomBox,		*zoomWindow,
				*computerBox,	*computerWindow,
				*displayBox,	*displayWindow,
				*playlistBox,	*playlistWindow,
				*digitsBox,		*digitsWindow;

extern WINDOW 	*ventana[2];
extern PANEL 	*panel[2];

extern Zoom zoom();

extern	short int x, y;
extern	short displayShowResults, playlistShowResults;

#endif