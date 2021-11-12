#include "zoomBox.h"
#include "../colors.h"
#include "../global.h"
#include "../common.h"

void ZoomBox::draw(const short toArray)
{
	if (toArray == QUERY) {
		box		= newwin(yRes * 44 / 200, 	  xRes * 120 / 200, 	2, 	 xRes * 00 / 200);
		window 	= newwin(yRes * 44 / 200 - 2, xRes * 120 / 200 - 2,	3, xRes * 00 / 200 + 1);
	}
	else {
		box		= newwin(yRes * 44 / 200, 	  xRes * 120 / 200, 	2, 	 xRes * 80 / 200);
		window 	= newwin(yRes * 44 / 200 - 2, xRes * 120 / 200 - 2,	3, xRes * 80 / 200 + 1);
	}

	wattron(box, COLOR_PAIR(GRAY_DEFAULT));
	wattron(box, A_BOLD);
	if (toArray == QUERY)
		wborder(box, 0, 0, 0, 0, ACS_ULCORNER, ACS_URCORNER, ACS_LTEE, ACS_BTEE);
	else
		wborder(box, 0, 0, 0, 0, ACS_ULCORNER, ACS_URCORNER, ACS_BTEE, ACS_RTEE);
	wrefresh(box);
}

void ZoomBox::erase(void)
{
	wclear(window);
	wclear(box);
	wrefresh(box);
}