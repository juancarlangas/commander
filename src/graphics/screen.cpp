#include "../common/math.hpp"
#include "../common/common.hpp"
#include "../data/databases.hpp"

extern short int x, y;
extern short int displayShowResults, playlistShowResults;

void decrease_index( int *top, int *index )/*{{{*/
{
	if ( *index > 0 ) { // que se pueda subir
		if ( *index == *top )
			( *top )--;
		( *index )--;
	}
}/*}}}*/

void increase_index(	int *top, const int resultRows, int *index, /*{{{*/
						const short int winMode	)
{
	short int showResults;

	if ( winMode == 1 )
		showResults = displayShowResults;
	else
		showResults = playlistShowResults;

	if (*index < resultRows - 1) {
		if (*index == *top + showResults - 1)
			(*top)++;
		(*index)++;
	}

	return;
}/*}}}*/

void korg_drag(	System table[],
				const int rows, const int indexUP, const int indexDOWN,
				const int caracter									)
{
	static int i;

	System store;

	if (caracter == 566) {
		if (rows >= static_cast<int32_t>( vabs(indexUP - indexDOWN) + 1) ) {
			store = table[indexUP - 1];
			for (i = indexUP; i <= indexDOWN; i++)
				table[i - 1] = table[i];
			table[indexDOWN] = store;
		}
	}
	
	else {
		if (rows >= static_cast<int32_t>( vabs(indexUP - indexDOWN) + 1) ) {
			store = table[indexDOWN + 1];
			for (i = indexDOWN; i >= indexUP; i--)
				table[i + 1] = table[i];
			table[indexUP] = store;
		}	
	}
	return;
}


