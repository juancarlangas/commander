#ifndef TABLES_HPP
#define TABLES_HPP

#include "databases.hpp"

short int compare(const char stringSystem[], const char stringUser[]);

void llenado_displayTable(	System *displayTable[], System dataBase[], 
							const int dbRows, const char string[],
							int *dRows);

void llenado_favourite(int fav[], System base[], const int baseRows);

short int compare(const char stringSystem[], const char stringUser[]);

#endif
