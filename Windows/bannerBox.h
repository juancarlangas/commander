#ifndef BANNERBOX_H
#define BANNERBOX_H

#include <ncurses.h>

class BannerBox {
public:
	void draw(const short);
	void erase(void);
private:
	WINDOW *box, *window;
};

#endif