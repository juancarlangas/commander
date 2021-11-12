#ifndef PLAYLISTBOX_H
#define PLAYLISTBOX_H

#include <ncurses.h>

class PlaylistBox {
public:
	void draw(const short);
	void erase(void);
private:
	WINDOW *box, *window;
};

#endif