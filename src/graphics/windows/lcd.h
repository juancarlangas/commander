#ifndef LCD_H
#define LCD_H

#include <ncurses.h>

class LCD {
public:
	void draw(void);
private:
	WINDOW *window;
};

#endif