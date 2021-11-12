#ifndef ZOOMBOX_H
#define ZOOMBOX_H

#include <ncurses.h>

class ZoomBox {
public:
	void draw(const short);
	void erase(void);
private:
	WINDOW *box, *window;
};

#endif