#include "tables.hpp"
#include <string.h>
#include "../common/common.hpp"
#include <stdlib.h>
#include <string.h>
#include <string>
#include <string_view>
#include "../common/string.hpp"

void llenado_displayTable(	System *_DisplayTable[], System _DataBase[], /*{{{*/
							const int _DBRows, const std::string_view _Cadena, int *_DisplayRows )
{
	switch ( _Cadena.data()[0] ) {

		case '\0': // RESET_QUERY
			*_DisplayRows = _DBRows; // reiniciamos
			for ( int32_t i = 0; i < _DBRows; ++i )
				_DisplayTable[i] = _DataBase + i;
			break;

		case '/': // Caracter de escape
			*_DisplayRows = 0;
			break;

		default: // Palabra
			*_DisplayRows = 0; // Reiniciamos

			for ( int32_t i = 0; i < _DBRows; ++i )
				if (	_DataBase[i].titulo.find( _Cadena ) != std::string::npos or
						_DataBase[i].artista.find( _Cadena ) != std::string::npos or
						_DataBase[i].genero.find( _Cadena ) != std::string::npos or
						_DataBase[i].mood.find( _Cadena ) != std::string::npos or
						_DataBase[i].key_words.find( _Cadena ) != std::string::npos ) {

					_DisplayTable[ *_DisplayRows ] = _DataBase + i;
					( *_DisplayRows )++;
				}
			break;
	}

	return;
}/*}}}*/

void llenado_favourite(int fav[], System base[], const int baseRows)/*{{{*/
{
	int i, count = 1;

	for (i = 0; i <= baseRows - 1; i++)
		if (strstr(base[i].keywords, "Favourite") != NULL)
			fav[count++ % 10] = i;

	return;
}/*}}}*/

short int compare(const char stringSystem[], const char stringUser[])/*{{{*/
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
}/*}}}*/
