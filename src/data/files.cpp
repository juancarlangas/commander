#include "files.hpp"
#include "../common/common.hpp"
#include "../common/string.hpp"

#include <stdlib.h>

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int load_playlist(System plArray[], const char* file_name)
{
	FILE *cfPointer;

	int rows;
	int i;

	//homedir
	const char *homedir;
 		if ((homedir = getenv("HOME")) == NULL)
			homedir = getpwuid(getuid())->pw_dir;

   	char path[80];
	sprintf(path, "%s/.commander/Playlists/%s.dat", homedir, file_name);
	cfPointer = fopen(path, "r");
		fscanf(cfPointer, "%d", &rows);
	fclose(cfPointer);

	sprintf(path, "%s/.commander/Playlists/%s.cpl", homedir, file_name);
	cfPointer = fopen(path, "r");
		for (i = 0; i <= rows - 1; i++)
			fread(&(plArray[i]), sizeof(System), 1, cfPointer);
	fclose(cfPointer);

	return rows;
}

void save_playlist(System plArray[], const int plRows, const char* file_name)
{
	FILE *cfPointer;

	int i;

	//homedir
	const char *homedir;
 		if ((homedir = getenv("HOME")) == NULL)
			homedir = getpwuid(getuid())->pw_dir;

   	char path[80];

	sprintf(path, "%s/.commander/Playlists/%s.dat", homedir, file_name);
	cfPointer = fopen(path, "w");
		fprintf(cfPointer, "%d", plRows);
	fclose(cfPointer);

	sprintf(path, "%s/.commander/Playlists/%s.cpl", homedir, file_name);
	cfPointer = fopen(path, "w");
		for (i = 0; i <= plRows - 1; i++)
			fwrite(&(plArray[i]), sizeof(System), 1, cfPointer);
	fclose(cfPointer);

	return;
}
