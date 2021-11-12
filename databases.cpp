#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#include "databases.h"
#include "form.h"
#include "string.h"

Database::Database()
{
	register int i, k;

	for (i = 0; i <= ROWS - 1; i++)
		clean_row(i);

	// homedir
	if ((homedir = getenv("HOME")) == NULL)
		homedir = getpwuid(getuid())->pw_dir;
}

///////////////////////// clean_row ///////////////////////
void Database::clean_row(int line)
{
	sprintf(base[line].title, "");
	sprintf(base[line].artist, "");
	sprintf(base[line].genre, "");
	sprintf(base[line].section, "");
	sprintf(base[line].keywords, "");
	sprintf(base[line].type, "");
	base[line].bnk = 0;
	base[line].num = 0;
}

/////////////////////// get_activeRows ////////////////////
int Database::get_activeRows(const char* folder, const char* name)
{
	char path[100];
	sprintf(path, "%s/%s.dat", folder, name);

	FILE *cFile = fopen(path, "r");
		fscanf(cFile, "%d", &activeRows);
	fclose(cFile);

	return activeRows;
}

/////////////////////////// load /////////////////////////
void Database::load(const char *directory, const char *filename)
{
	char path[100];
	sprintf(path, "%s/%s.csv", directory, filename);

	FILE *cFile = fopen(path, "r");

	register int i;
	int k;
	char c, quote, rawstring[LONG_STRING];

	for (i = 0; i <= activeRows - 1; i++) {

		//*************title***************
		fscanf(cFile, "%c", &c);
		if (c == '\"') {
			quote = '\"';
			k = 0;
		}
		else {
			quote = ',';
			rawstring[0] = c;
			k = 1;
		}
		while (k == 0 || (k > 0 && rawstring[k - 1] != quote)) {
			fscanf(cFile, "%c", &c);
			rawstring[k++] = c;
		}
		rawstring[k - 1] = '\0';
		no_accent(base[i].title, rawstring);
		if (quote == '\"')
			fscanf(cFile, ",");

		//**************artist***********************
		fscanf(cFile, "%c", &c);
		if (c == '\"') {
			quote = '\"';
			k = 0;
		}
		else {
			quote = ',';
			rawstring[0] = c;
			k = 1;
		}
		while (k == 0 || (k > 0 && rawstring[k - 1] != quote)) {
			fscanf(cFile, "%c", &c);
			rawstring[k++] = c;
		}
		rawstring[k - 1] = '\0';
		no_accent(base[i].artist, rawstring);
		if (quote == '\"')
			fscanf(cFile, ",");

		//**************genre***********************
		fscanf(cFile, "%c", &c);
		if (c == '\"') {
			quote = '\"';
			k = 0;
		}
		else {
			quote = ',';
			rawstring[0] = c;
			k = 1;
		}
		while (k == 0 || (k > 0 && rawstring[k - 1] != quote)) {
			fscanf(cFile, "%c", &c);
			rawstring[k++] = c;
		}
		rawstring[k - 1] = '\0';
		no_accent(base[i].genre, rawstring);
		if (quote == '\"')
			fscanf(cFile, ",");

		//**************section***********************
		fscanf(cFile, "%c", &c);
		if (c == '\"') {
			quote = '\"';
			k = 0;
		}
		else {
			quote = ',';
			rawstring[0] = c;
			k = 1;
		}
		while (k == 0 || (k > 0 && rawstring[k - 1] != quote)) {
			fscanf(cFile, "%c", &c);
			rawstring[k++] = c;
		}
		rawstring[k - 1] = '\0';
		no_accent(base[i].section, rawstring);
		if (quote == '\"')
			fscanf(cFile, ",");

		//**************keywords***********************
		fscanf(cFile, "%c", &c);
		if (c == '\"') {
			quote = '\"';
			k = 0;
		}
		else {
			quote = ',';
			rawstring[0] = c;
			k = 1;
		}
		while (k == 0 || (k > 0 && rawstring[k - 1] != quote)) {
			fscanf(cFile, "%c", &c);
			rawstring[k++] = c;
		}
		rawstring[k - 1] = '\0';
		no_accent(base[i].keywords, rawstring);
		if (quote == '\"')
			fscanf(cFile, ",");

		//************** type ***********************
		fscanf(cFile, "%c", &c);
		if (c == '\"') {
			quote = '\"';
			k = 0;
		}
		else {
			quote = ',';
			rawstring[0] = c;
			k = 1;
		}
		while (k == 0 || (k > 0 && rawstring[k - 1] != quote)) {
			fscanf(cFile, "%c", &c);
			rawstring[k++] = c;
		}
		rawstring[k - 1] = '\0';
		no_accent(base[i].type, rawstring);
		if (quote == '\"')
			fscanf(cFile, ",");
		
		//****************bnk*******************
		fscanf(cFile, "%c", &c);
		if (c != ',') {
			base[i].bnk = c;
			fscanf(cFile, ",");
		}

		//************** num ***********************
		fscanf(cFile, "%c", &c);
		if (c != ',') {
			rawstring[0] = c;
			for (k = 1; k <= 2; k++) {
				fscanf(cFile, "%c", &c);
				rawstring[k] = c;
			}
			fscanf(cFile, ",");
		}
		rawstring[k] = '\0';
		base[i].num = atoi(rawstring);

		//******************************************
		fscanf(cFile, "\n");
	}

	fclose(cFile);
}

///////////////////////////// exportate ////////////////////////
void Database::exportate(const char *filename)
{
	char path[100];
	sprintf(path, "%s/.commander/%s.dat", homedir, filename);

	FILE *cFile = fopen(path, "w");
		fprintf(cFile, "%d", activeRows);
	fclose(cFile);

	sprintf(path, "%s/.commander/%s.csv", homedir, filename);

	cFile = fopen(path, "w");

	register int i;
	int k;
	char c, quote, rawstring[LONG_STRING];

	for (i = 0; i <= activeRows - 1; i++) {

		//*************title************
		if (strstr(base[i].title, ","))
			fprintf(cFile, "\"%s\",", base[i].title);
		else
			fprintf(cFile, "%s,", base[i].title);

		if (strstr(base[i].artist, ","))
			fprintf(cFile, "\"%s\",", base[i].artist);
		else
			fprintf(cFile, "%s,", base[i].artist);

		fprintf(cFile, "%s,", base[i].genre);
		fprintf(cFile, "%s,", base[i].section);
		fprintf(cFile, "%s,", base[i].keywords);
		fprintf(cFile, "%s,", base[i].type);
		fprintf(cFile, "%c,", base[i].bnk);
		fprintf(cFile, "%03d\n", base[i].num);
	}
	fclose(cFile);
		
}


///////////////////////// add_value ////////////////////////
void Database::add_value(System row)
{
	base[activeRows++] = row;
}

/////////////////////// edit_value //////////////////////////////7
void Database::edit_value(int line, System row)
{
	base[line] = row;
}

//////////////////////// delete_value ////////////////////////
void Database::delete_value(int line)
{
	for (int i = line; i < activeRows - 1; i++)
		base[i] = base[i + 1];

	clean_row(activeRows--);
}

////////////////////////// ordenate /////////////////////////////7
void Database::ordenate()
{
	register int a, b;
	register short k;
	bool success;
	System aux;

	//Sounds	
	a = 0;
	b = a + 1;
	while (a <= activeRows - 2 && b <= activeRows - 1) {	
		if (strstr(base[a].section, "Sound") == NULL || strstr("Sound", base[a].section) == NULL) {
			b = a + 1;
			success = false;
			while (success == false && b <= activeRows - 1) {
				if (strstr(base[b].section, "Sound") == NULL || strstr("Sound", base[b].section) == NULL)
					b++;
				else {
					aux = base[a];
					base[a] = base[b];
					base[b] = aux;
					success = true;
				}
			}
		}
		a++;
	}

	//Lobby
	a--;
	b = a + 1;
	while (a <= activeRows - 2 && b <= activeRows - 1) {	
		if (strstr(base[a].section, "Lobby") == NULL || strstr("Lobby", base[a].section) == NULL) {
			b = a + 1;
			success = false;
			while (success == false && b <= activeRows - 1) {
				if (strstr(base[b].section, "Lobby") == NULL || strstr("Lobby", base[b].section) == NULL)
					b++;
				else {
					aux = base[a];
					base[a] = base[b];
					base[b] = aux;
					success = true;
				}
			}
		}
		a++;
	}

	//Cena
	a--;
	b = a + 1;
	while (a <= activeRows - 2 && b <= activeRows - 1) {	
		if (strstr(base[a].section, "Cena") == NULL || strstr("Cena", base[a].section) == NULL) {
			b = a + 1;
			success = false;
			while (success == false && b <= activeRows - 1) {
				if (strstr(base[b].section, "Cena") == NULL || strstr("Cena", base[b].section) == NULL)
					b++;
				else {
					aux = base[a];
					base[a] = base[b];
					base[b] = aux;
					success = true;
				}
			}
		}
		a++;
	}

	//Baile
	a--;
	b = a + 1;
	while (a <= activeRows - 2 && b <= activeRows - 1) {	
		if (strstr(base[a].section, "Baile") == NULL || strstr("Baile", base[a].section) == NULL) {
			b = a + 1;
			success = false;
			while (success == false && b <= activeRows - 1) {
				if (strstr(base[b].section, "Baile") == NULL || strstr("Baile", base[b].section) == NULL)
					b++;
				else {
					aux = base[a];
					base[a] = base[b];
					base[b] = aux;
					success = true;
				}
			}
		}
		a++;
	}

	//ALphabet
	for (a = 0; a <= activeRows - 2; a++)
		for (b = a + 1; b <= activeRows - 1; b++) {
			if (strstr(base[a].section, base[b].section) &&
				strstr(base[b].section, base[a].section))  {
				k = 0;
				success = false;
				while (success == false && k < LONG_STRING) {
					if (base[a].title[k] > base[b].title[k]) { // cambio y EXITO
						aux 	= base[a];
						base[a] = base[b];
						base[b] = aux;
						success = true;		
					} else if (base[a].title[k] < base[b].title[k]) // se queda pero EXITO
						success = true;
						
					k++;
				}
			}
		}
}
