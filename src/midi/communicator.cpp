#include "communicator.hpp"
#include "../common/common.hpp"
#include "audio.hpp"

#include <string.h>
#include <time.h>

void communicator(Keyboard& device, System *row, Variation var, short channel)
{
	static struct timespec combiTimer, multiTimer;
   	 		combiTimer.tv_sec 	= 0;
   			combiTimer.tv_nsec	= 200000000;
   			multiTimer.tv_sec 	= 0;
   			multiTimer.tv_nsec 	= 2000000000;

	/**************** Combination ******************/
	//if (strstr("prg", row->type) != NULL) {
		/* mode:Comb */
		//device.set_modality(COMBI);
		//nanosleep(&combiTimer, NULL);

		/* bnk/prg */
		if (row->bnk != '\0')
			switch (var) {
				case VAR1:
					device.set_program(row->bnk, row->num);
					break;
				case VAR2:
					device.set_program(row->bnk, row->num);
					break;
			}
	//}
	/******************* Multi Song ***************
	else if (strstr("mid", row->type) != NULL) {

		/* mode:Multi *
		device.set_modality(MULTI);
		nanosleep(&multiTimer, NULL);

		/* title Select *
		if (row->bnk1 == 'M') {
			device.set_song(row->num1);
			nanosleep(&multiTimer, NULL);
		}
		/* play *
		if (row->title[0] != '\0');
			device.play_midi_seq(row->title);


	}

	/******************* media Song ***************
	else if (	strstr("wav", row->type) != NULL ||
				strstr("mp3", row->type) != NULL ||
				strstr("m4a", row->type) != NULL ||
				strstr("ogg", row->type) != NULL ||
				strstr("mp4", row->type) != NULL ||
				strstr("webm",row->type) != NULL 	) {

		/* mode:Comb *
		device.set_modality(COMBI);
		nanosleep(&combiTimer, NULL);

		/* bnk/prg *
		if (row->bnk1 != '\0');
			device.set_program(row->bnk1, row->num1);

		/* play *
		if (row->title[0] != '\0');									
			play_media_seq(row->title, row->type);
	}*/

	return;
}
