#include <ctype.h>
#include "common/string.hpp"

int no_accent(char after[], const char prev[])/*{{{*/
{
	int i, j;
	int c_plus;

	/* Copy */
	i = 0;
	do { 	//copia cadena
		after[i] = prev[i];
	} while (after[i++] != '\0');

	/*Engine */
	i = 0;
	do {
		/*Replace*/
		if (after[i] == -61) { /*especial*/
			c_plus = after[i + 1];
			switch (c_plus) {
				case -127:
					after[i] = 'A';
					break;
				case -119:
					after[i] = 'E';
					break;
				case -115:
					after[i] = 'I';
					break;
				case -109:
					after[i] = 'O';
					break;
				case -102:
					after[i] = 'U';
					break;
				case -111:
					after[i] = 'N';
					break;
				case -95:
					after[i] = 'a';
					break;
				case -87:
					after[i] = 'e';
					break;
				case -83:
					after[i] = 'i';
					break;
				case -77:
					after[i] = 'o';
					break;
				case -70:
					after[i] = 'u';
					break;
				case -79:
					after[i] = 'n';
					break; 
				/*case '\0':
					after[i] = '\0';
					break;*/
				default:
					break;
			}
			/* Carrousel */
			j = i; 
			do {
				after[j + 1] = after[j + 2];
				j++;
			} while (after[j] != '\0');
		}

		if (after[i] == '\'' || after[i] == '\\') { /*tilde*/
			j = i; 
			do {
				after[j] = after[j + 1];
			} while (after[j++] != '\0');
		}
		
	} while (after[i++] != '\0'); 

	return i - 1;
}/*}}}*/

void low_string(char cadena[])/*{{{*/
{
	short i = 0;

	do {
		cadena[i] = tolower( (int)cadena[i] );
	} while ( cadena[i++] != '\0' );

	return;
}/*}}}*/

void up_string(char cadena[])/*{{{*/
{
	short i = 0;

	do {
		cadena[i] = toupper((int)cadena[i]);
	} while (cadena[i++] != '\0');

	return;
}/*}}}*/

std::string low_string( std::string_view _Cadena ) noexcept/*{{{*/
{
	std::string cadena_temporal;

	for ( int32_t i = 0; i < static_cast<int32_t>( _Cadena.length() ); ++ i )
			cadena_temporal = cadena_temporal +
				static_cast<char>( std::tolower( _Cadena.data()[i] ) );

	return cadena_temporal;
}/*}}}*/
