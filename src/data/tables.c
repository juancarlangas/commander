#include <string.h>
#include "../common/common.h"
#include <stdlib.h>
#include <string.h>
#include "../common/string.h"
#include "databases.h"

short int compare(const char stringSystem[], const char stringUser[]);

/*/////////////////////////////////////// llenado_displayTable ////////////////////////////////////////*/
void 	llenado_displayTable(	System *displayTable[], System dataBase[], 
								const int dbRows, const char string[],
								int *dRows)
{
	short int match;
	register short int i, iDisplay, j, k;
	short int seekTop, seekBottom;

	switch (string[0]) {
		case '\0': /*CLEAN*/
			*dRows = dbRows;
			for (i = 0; i <= dbRows - 1; i++)
				displayTable[i] = dataBase + i;
			break;

		case '/':
			*dRows = 0;
			break;

		default: /*WORD*/
			*dRows = 0;

			for (i = 0; i <= dbRows - 1; i++) {
				match = 0;

				match += compare(dataBase[i].title, string);
				match += compare(dataBase[i].artist, string);
				match += compare(dataBase[i].genre, string);
				match += compare(dataBase[i].section, string);
				match += compare(dataBase[i].keywords, string);

				if (match) {
					(*dRows)++;
					displayTable[*dRows - 1] = dataBase + i;
				}
			}
			break;
	}

	return;
}

/************************************** llenado_favourite ***************************************/
void llenado_favourite(int fav[], System base[], const int baseRows)
{
	int i, count = 1;

	for (i = 0; i <= baseRows - 1; i++)
		if (strstr(base[i].keywords, "Favourite") != NULL)
			fav[count++ % 10] = i;

	return;
}

/****************************************** compare **************************************************/
short int compare(const char stringSystem[], const char stringUser[])
{
	char wordSystem[LONG_STRING], wordUser[LONG_STRING];
	short int check;

	no_accent(wordSystem, stringSystem);
	low_string(wordSystem);
	no_accent(wordUser, stringUser);
	low_string(wordUser);

	if (strstr(wordSystem, wordUser) != NULL)
		check = 1;
	else
		check = 0;

	return check;
}
