#include <stdlib.h>

#include "channelForm.h"
#include "colors.h"

extern short int x, y;

ChannelForm::ChannelForm()
{
    windowFrame	= newwin(y * 60 / 200, x * 100 / 200, y * 30 / 200, x * 40 / 200);
    panelFrame 	= new_panel(windowFrame);

    windowInput	= newwin(y * 60 / 200 + 1, x * 100 / 200 + 10, 1, 2);
    panelInput	= new_panel(windowInput);

    wattron(windowFrame, COLOR_PAIR(GRAY_DEFAULT));
    wattron(windowFrame, A_BOLD);
	box(windowFrame, 0, 0);
	mvwprintw(windowFrame, 1, 1, "Channel:");

	wattron(windowInput, COLOR_PAIR(BLACK_GRAY));
	wattron(windowInput, A_BOLD);
}


////////////////////////// ask_midiChannel //////////////////////////////
short ChannelForm::ask_channel(short channel)
{
	show_panel(panelFrame);
	show_panel(panelInput);

	mvwprintw(windowInput, 0, 0, "%02hd", channel);

	enum Order {IGNORE, UPDATE, A_LA_VERGA_COMPADRE, AMONOS_A_LA_CHINGADA} order;

	short propose;
	
	int key = getch();
	do {
		switch (key) {
			case '\n': //ENTER
				order = AMONOS_A_LA_CHINGADA;
				break;
			case 27: //ESCAPE
				order = A_LA_VERGA_COMPADRE;
				break;
			default:
				if (key >= 48 && key <= 57) {
					propose = channel * 10 % 100 + (key - 48);
					if (propose >= 1 && propose <= 16)
						order = UPDATE;
				}
		}
	} while (order == UPDATE || order == IGNORE);

	return 0;
}