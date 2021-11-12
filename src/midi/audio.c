/* This include "system()" dedicated functions to manipulate via command line 	*/
/* the 'pmidi' and 'mplayer' Linux programs										*/

#include <stdlib.h>
#include <ncurses.h>

/*********************************** snd_audio_seq ***********************************/
void play_media_seq(char *song, char *type)
{
	char command[150] = "";

	sprintf(command, "mplayer -ao jack:port=mono -vo xv -fs '/media/juancarlangas/KINGSTON/Music/export/media/%s.%s'", song, type);

	def_prog_mode();
	endwin();
		system(command);
	reset_prog_mode();
	refresh();

	return;
}
