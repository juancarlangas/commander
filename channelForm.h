#ifndef CHANNELFORM_H
#define CHANNELFORM_H

#include <ncurses.h>
#include <panel.h>

class ChannelForm {
public:
	ChannelForm();
	short ask_channel(short);
private:
    WINDOW  *windowFrame, 	*windowInput;
    PANEL   *panelFrame, 	*panelInput;
};

#endif