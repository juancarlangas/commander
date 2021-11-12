#ifndef DATABASES_H
#define DATABASES_H

#include "common.h"

#define ROWS 1000

struct System {
		char title[LONG_STRING];
		char artist[LONG_STRING];
		char genre[LONG_STRING];
		char section[LONG_STRING];
		char keywords[LONG_STRING];
		char type[10];
		char bnk;
		short num;
	};

class Database {
public:
	Database();
	void load(const char*, const char *);
	int get_activeRows(const char *,const char *);
	void add_value(System);
	void edit_value(int, System);
	void delete_value(int);
	void ordenate();
	void exportate(const char *);
	System base[ROWS];
private:
	void clean_row(int);
	const char *homedir;
	int activeRows;
};

#endif