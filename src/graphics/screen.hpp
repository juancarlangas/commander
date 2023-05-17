#ifndef SCREEN_HPP
#define SCREEN_HPP

#include "../data/databases.hpp"

void decrease_index( int *top, int *index );

void increase_index(	int *top, const int resultRows, int *index, 
						const short int winMode	);

void korg_drag(	Performance table[],
				const int rows, const int indexUP, const int indexDOWN,
				const int caracter );

#endif
