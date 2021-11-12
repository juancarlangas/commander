#include "fields.h"
#include "common.h"
#include "colors.h"
#include "string.h"

void Field::create(	const char* legend,
					const int high, const int width, const int y, const int x,
					const Type tipo, const short limite)
{
	wCaja = newwin(high, width, y, x);
	pCaja = new_panel(wCaja);

	wEspacio = newwin(high - 2, width - 2, y + 1, x + 1);
	pEspacio = new_panel(wEspacio);

	type = tipo;
	limit = limite;
	
	wattron(wCaja, COLOR_PAIR(GRAY_DEFAULT));
	wattron(wCaja, A_BOLD);
	box(wCaja, 0, 0);
	mvwprintw(wCaja, 0, 1, " %s ", legend);

	wattron(wEspacio, COLOR_PAIR(WHITE_DEFAULT));
}

////////////////////////////////// show ////////////////////
void Field::show()
{
	show_panel(pCaja);
	show_panel(pEspacio);
}

/////////////////////////////// hide ////////////////////////
void Field::hide()
{
	hide_panel(pCaja);
	hide_panel(pEspacio);
}

/////////////////////////////// set_cursor //////////////////
void Field::get_cursor()
{
	wprintw(wEspacio, "");
	wrefresh(wEspacio);
}

//////////////////////////////////// set_string(const char *) ////////////////////////////7
void Field::set_string(const char *word)
{
	sprintf(cadena, "%s", word);
	nCaracter = 0;

	while (word[nCaracter] != '\0') {
		cadena[nCaracter] = word[nCaracter];
		nCaracter++;
	}

	wclear(wEspacio);
	wprintw(wEspacio, "%s", cadena);
	wrefresh(wEspacio);
}

//////////////////////////////////// set_string(const char) ////////////////////////////7
void Field::set_string(const char digit)
{
	cadena[0] = digit;
	cadena[1] = '\0';

	wclear(wEspacio);
	wprintw(wEspacio, "%s", cadena);
	wrefresh(wEspacio);
}

//////////////////////////////////// set_string(const short) ////////////////////////////7
void Field::set_string(const short number)
{
	sprintf(cadena, "%03d", number); 

	wclear(wEspacio);
	wprintw(wEspacio, "%s", cadena);
	wrefresh(wEspacio);
}

///////////////////////////// process_key ///////////////////////
bool Field::process_key(int tecla)
{
	bool success = false;

	if (nCaracter < limit) 
		switch (type) {
			case STRING:
				if ((tecla == 32) || (tecla >= 44 && tecla <= 122)) { //standard//
					cadena[nCaracter++] = tecla;
					cadena[nCaracter]	= '\0';
					success = true;
				} else if (nCaracter >= 1) { //backspace//
					cadena[--nCaracter] = '\0';
					success = true;
				}
				break;

			case BANK:
				if (tecla >= 97 && tecla <= 122) { //if lowercase
					tecla -= 32;
					success = true;
				}
				if (tecla >= 65 && tecla <= 90) { //uppercase
					cadena[0] = tecla;
					cadena[1] 	= '\0';
					success = true;
				}
				 
				break;

			case VALUE:
				if (tecla >= 48 && tecla <= 57) { //number
					cadena[0] = cadena[1];
					cadena[1] = cadena[2];
					cadena[2] = tecla;
					success = true;
				} else if (nCaracter >= 1) { //backspace//
					cadena[2] = cadena[1];
					cadena[1] = cadena[0];
					cadena[2] = '0';
					success = true;
				}
				break;
	}

	if (success == true) {
		wclear(wEspacio);
		wprintw(wEspacio, "%s", cadena);
		wrefresh(wEspacio);
	}

	return true;
}

///////////////////////////////// get_string() //////////////////////////////////
void Field::get_string(char word[])
{
	sprintf(word, "%s", cadena);
}