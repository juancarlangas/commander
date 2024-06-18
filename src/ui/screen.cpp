#include "../common/math.hpp"
#include "../data/catalog.hpp"
#include "common/common.hpp"
#include <cstdint>

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

void increase_index(int *i_top, const int total_results,
		int *abs_index, const std::int16_t winMode) {
	std::int16_t shown_results {winMode == MODE_DISPLAY ? displayShowResults
														: playlistShowResults};

	if (*abs_index < total_results - 1) {
		if (*abs_index == *i_top + shown_results - 1)
			++(*i_top);
		++(*abs_index);
	}
}

void korg_drag(	Performance table[],/*{{{*/
				const int rows, const int indexUP, const int indexDOWN,
				const int caracter									)
{
	static int i;

	Performance store;

	if (caracter == 566) {
		if (rows >= static_cast<int32_t>(vabs(indexUP - indexDOWN) + 1)) {
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
}/*}}}*/


