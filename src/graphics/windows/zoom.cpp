#include "zoom.h"
#include "../colors.h"

Zoom::Zoom(const int heigh, const int width, const int yPos, const int xPos)
{
	box		= newwin(heigh, 	width, 		yPos, 		xPos);
	window 	= newwin(heigh - 2, width - 2, 	yPos + 1, 	xPos + 1);
}

void Zoom::draw(void)
{
	wattron(box, COLOR_PAIR(GRAY_DEFAULT));
	wattron(box, A_BOLD);
	wborder(box, 0, 0, 0, 0, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
	wrefresh(box);
}