#ifndef ZOOM_H
#define ZOOM_H

#include <ncurses.h>

class Zoom {
public:
	Zoom(const int, const int, const int, const int);
	void draw(void);
private:
	WINDOW *box, *window;
};

#endif