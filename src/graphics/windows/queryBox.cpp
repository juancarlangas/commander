#include "queryBox.h"
#include "../colors.h"
#include "../global.h"

void QueryBox::draw(void) 
{
	box 	= newwin(6 *  1 / 200,	   xRes * 140 / 200,	 yRes * 84 / 100, 	  xRes *  1 / 200);
	window 	= newwin(6 *  1 / 200 - 2, xRes * 140 / 200 - 2, yRes * 84 / 100 - 2, xRes *  1 / 200 - 2);

	wattron(box, COLOR_PAIR(BLUE_DEFAULT));
	wattron(box, A_BOLD);		
	wborder(box, ' ', ' ', 0, 0, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
	wrefresh(box);
	wattron(box, COLOR_PAIR(BLUE_DEFAULT));
	wattron(box, A_BOLD);
}