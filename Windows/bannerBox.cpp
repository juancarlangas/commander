#include "bannerBox.h"
#include "../colors.h"
#include "../common.h"
#include "../global.h"

void BannerBox::draw(const short toArray)
{
	if (toArray == QUERY) {
		box		= newwin(3, 	  xRes * 120 / 200,	0, xRes * 00 / 200);
		window 	= newwin(1, xRes * 120 / 200 - 2,	1, xRes * 00 / 200 + 1);
	}
	else if (toArray == PLAYLIST) {
		box		= newwin(3, 	  xRes * 120 / 200, 0, xRes * 80 / 200);
		window 	= newwin(1, xRes * 120 / 200 - 2,	1, xRes * 80 / 200 + 1);
	}

	wattron(box, COLOR_PAIR(GRAY_DEFAULT));
	wattron(box, A_BOLD);
	wborder(box, 0, 0, 0, 0, ACS_ULCORNER, ACS_URCORNER, ACS_LTEE, ACS_RTEE);
	wrefresh(box);

	wattron(window, COLOR_PAIR(GRAY_DEFAULT));
	wattron(window, A_BOLD);
	mvwprintw(window, 0, 2, "Now playing:");
	wrefresh(window);
}

void BannerBox::erase(void)
{
	wclear(window);
	wclear(box);
	wrefresh(box);
}
