#include "playlistBox.h"
#include "../common.h"
#include "../global.h"
#include "../colors.h"

void PlaylistBox::draw(const short toArray)
{
	box		= newwin(yRes * 78 / 100, 	  xRes *  61 / 200,		yRes * 24 / 100, 	 xRes * 70 / 100);
	window 	= newwin(yRes * 78 / 100 - 2, xRes *  61 / 200 - 2,	yRes * 24 / 100 - 2, xRes * 70 / 100 - 2);

	wattron(box, COLOR_PAIR(GRAY_DEFAULT));
	wattron(box, A_BOLD);
	if (toArray == QUERY)
		wborder(box, 0, 0, 0, 0, ACS_TTEE, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
	else
		wborder(box, 0, 0, 0, 0, ACS_TTEE, ACS_RTEE, ACS_LLCORNER, ACS_LRCORNER);
	mvwprintw(box, 1, 12, " PlayList ");
	wrefresh(box);
}

void PlaylistBox::erase(void)
{
	wclear(window);
	wclear(box);
	wrefresh(box);
}