#ifndef RESULTSBOX_H
#define RESULTSBOX_H

#include <ncurses.h>

class ResultsBox {
public:
	void draw(const short);
	void erase(void);
private:
	WINDOW *box, *window;
};

#endif