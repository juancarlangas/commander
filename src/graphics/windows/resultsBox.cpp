#include "resultsBox.h"
#include "../colors.h"
#include "../common.h"
#include "../global.h"

void ResultsBox::draw(const short toArray)
{
	box		= newwin(yRes * 62 / 100, 	  xRes * 141 / 200, 	yRes * 24 / 100,	 xRes * 00 / 100);
	window 	= newwin(yRes * 62 / 100 - 2, xRes * 141 / 200 - 2, yRes * 24 / 100 - 2, xRes * 00 / 100 - 2);

	wattron(box, COLOR_PAIR(GRAY_DEFAULT));
	wattron(box, A_BOLD);
	if (toArray == QUERY)
		wborder(box, 0, 0, 0, 0, ACS_LTEE, ACS_TTEE, ACS_LLCORNER, ACS_RTEE);
	else
		wborder(box, 0, 0, 0, 0, ACS_ULCORNER, ACS_TTEE, ACS_LLCORNER, ACS_RTEE);
	mvwprintw(box, 1, 26, " SONG ");
	mvwprintw(box, 1, 32, " ARTIST ");
	mvwprintw(box, 1, 59, " GENRE ");
	mvwprintw(box, 1, 76, " KEYWORDS ");
	wrefresh(box);
}


void ResultsBox::erase(void)
{
	wclear(window);
	wclear(box);
	wrefresh(box);
}