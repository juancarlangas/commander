#ifndef QUERYBOX_H
#define QUERYBOX_H

#include <ncurses.h>

class QueryBox {
public:
	void draw(void);
private:
	WINDOW *box, *window;
};

#endif