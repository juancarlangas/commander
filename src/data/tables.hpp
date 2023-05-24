#ifndef TABLES_HPP
#define TABLES_HPP

#include "catalog.hpp"

short int compare(const char stringSystem[], const char stringUser[]);

void llenado_displayTable(	Performance *displayTable[], std::vector<Performance> &dataBase,
							const int dbRows, const std::string_view _Cadena,
							int *dRows);

void llenado_favourite(int fav[], Performance base[], const int baseRows);

short int compare(const char stringSystem[], const char stringUser[]);

#endif
