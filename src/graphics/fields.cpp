#include "fields.hpp"
#include "../common/common.hpp"
#include "../graphics/colors.hpp"
#include "../common/string.hpp"
#include <curses.h>
#include <iostream>
#include <string_view>
#include <string.h>

void Field::create(	const char* legend,/*{{{*/
					const int high, const int width, const int _Y, const int _X,
					const Type tipo, const short limite)
{
	wCaja = newwin(high, width, _Y, _X);
	pCaja = new_panel(wCaja);

	wEspacio = newwin(high - 2, width - 2, _Y + 1, _X + 1);
	pEspacio = new_panel(wEspacio);

	type = tipo;
	limit = limite;
	
	wattron(wCaja, COLOR_PAIR(GRAY_DEFAULT));
	wattron(wCaja, A_BOLD);
	box(wCaja, 0, 0);
	mvwprintw(wCaja, 0, 1, " %s ", legend);

	wattron(wEspacio, COLOR_PAIR(WHITE_DEFAULT));
}/*}}}*/

void Field::show()/*{{{*/
{
	show_panel(pCaja);
	show_panel(pEspacio);
}/*}}}*/

void Field::hide()/*{{{*/
{
	hide_panel(pCaja);
	hide_panel(pEspacio);
}/*}}}*/

void Field::get_cursor()/*{{{*/
{
	wprintw(wEspacio, "");
	curs_set( true );
	wrefresh(wEspacio);
}/*}}}*/

void Field::set_cursor() noexcept/*{{{*/
{
	get_cursor();
}/*}}}*/

void Field::set_string(const char *word)/*{{{*/
{
	sprintf(cadena, "%s", word);
	nCaracter = 0;

	while (word[nCaracter] != '\0') {
		cadena[nCaracter] = word[nCaracter];
		nCaracter++;
	}

	wclear(wEspacio);
	waddstr(wEspacio, cadena);
	wrefresh(wEspacio);
}/*}}}*/

void Field::set_string(const char digit)/*{{{*/
{
	cadena[0] = digit;
	cadena[1] = '\0';

	wclear(wEspacio);
	waddstr(wEspacio, cadena);
	wrefresh(wEspacio);
}/*}}}*/

void Field::set_string(const short number)/*{{{*/
{
	sprintf(cadena, "%03d", number); 

	wclear(wEspacio);
	wprintw(wEspacio, "%s", cadena);
	wrefresh(wEspacio);
}/*}}}*/

void Field::set_content( const std::string &_Cadena ) noexcept/*{{{*/
{
	strcpy( cadena, _Cadena.c_str() );

	wclear( wEspacio );
	wprintw( wEspacio, "%s", cadena );
	wrefresh( wEspacio );
}/*}}}*/

bool Field::process_key(int tecla)/*{{{*/
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
}/*}}}*/

void Field::get_string(char word[])/*{{{*/
{
	sprintf(word, "%s", cadena);
}/*}}}*/

void Field::set_font_color( const int32_t &_Color ) noexcept/*{{{*/
{
	wattron( wEspacio, COLOR_PAIR( _Color ) );
}/*}}}*/

void Field::set_font_width( const std::string_view _Style ) noexcept/*{{{*/
{
	if ( _Style != "Bold" and _Style != "Regular" ) {
		std::cerr << _Style << " no se reconoce como un estilo en Field::set_font_width()"
			<< std::endl;
		exit( EXIT_FAILURE );
	}

	if ( _Style == "Regular" )
		wattroff( wEspacio, A_BOLD );
	else
		wattron( wEspacio, A_BOLD );
}/*}}}*/
