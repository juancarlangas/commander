#include <bits/stdint-intn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>
#include <string_view>
#include <iostream>
#include <fstream>
#include <iomanip> // zerofill

#include "databases.hpp"
#include "form.h"
#include "../common/string.hpp"

Database::Database() : activeRows( 0 )
{
	int32_t i;

	for (i = 0; i <= ROWS - 1; i++)
		clean_row(i);

	// homedir
	if ((homedir = getenv("HOME")) == NULL)
		homedir = getpwuid(getuid())->pw_dir;
}

void Database::clean_row(int line)
{
	*base[line].title = '\0';
	*base[line].artist = '\0';
	*base[line].genre = '\0';
	*base[line].section = '\0';
	*base[line].keywords = '\0';
	*base[line].type = '\0';
	base[line].bnk = 0;
	base[line].num = 0;
}

void Database::cargar( const std::string &_Path ) noexcept
{
	std::ifstream archivo{ _Path + "/combinations.csv" };
	std::ofstream log{ _Path + "/log.txt" };
	
	if ( archivo.fail() ) {
		std::cerr << "No se pudo abrir " + _Path + "en BaseDeDatos::cargar()" << std::endl;
		exit( EXIT_FAILURE );
	}

	activeRows = 0;
	std::string linea;

	int32_t n_linea = 0;
	while ( std::getline( archivo, linea ) or linea != "" ) {
		++activeRows;

		// Titulo
		if ( linea.starts_with( '\"' ) ) { // Incluye comas
			linea = linea.substr( 1 ); // Quitamos "
			catalogo[n_linea].titulo = linea.substr( 0, linea.find_first_of( '\"' ) );
			linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
		}
		else {
			catalogo[n_linea].titulo = linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 );
		}

		// Artista
		if ( linea.starts_with( '\"' ) ) { // Incluye comas
			linea = linea.substr( 1 ); // Quitamos "
			catalogo[n_linea].artista = linea.substr( 0, linea.find_first_of( '\"' ) );
			linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
		}
		else {
			catalogo[n_linea].artista = linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 );
		}

		// Genero
		if ( linea.starts_with( '\"' ) ) { // Incluye comas
			linea = linea.substr( 1 ); // Quitamos "
			catalogo[n_linea].genero = linea.substr( 0, linea.find_first_of( '\"' ) );
			linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
		}
		else {
			catalogo[n_linea].genero = linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 );
		}

		// Mood
		if ( linea.starts_with( '\"' ) ) { // Incluye comas
			linea = linea.substr( 1 ); // Quitamos "
			catalogo[n_linea].mood = linea.substr( 0, linea.find_first_of( '\"' ) );
			linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
		}
		else {
			catalogo[n_linea].mood = linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 );
		}

		// Keywords
		if ( linea.starts_with( '\"' ) ) { // Incluye comas
			linea = linea.substr( 1 ); // Quitamos "
			catalogo[n_linea].key_words = linea.substr( 0, linea.find_first_of( '\"' ) );
			linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
		}
		else {
			catalogo[n_linea].key_words = linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 );
		}

		// Tipo
		catalogo[n_linea].tipo = linea.substr( 0, linea.find_first_of( ',' ) );
		linea = linea.substr( linea.find_first_of( ',' ) + 1 );

		// BNK
		base[n_linea].bnk = linea.data()[0];
		linea = linea.substr( 2 ); // garantiza que queda después de la 'coma'

		// NUM
		base[n_linea].num = std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) );
		linea = linea.substr( linea.find_first_of( ',' ) + 1 );

		// n_arreglos
		catalogo[n_linea].n_arreglos = std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) );
		linea = linea.substr( 2 );

		for ( int32_t i = 0; i < catalogo[n_linea].n_arreglos; ++i ) {
			// Label
			catalogo[n_linea].orquestacion[i].etiqueta =
				linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 ); // 1 después de la 'coma'
			// data
			for ( int32_t j = 0; j < 8; ++j ) {
				catalogo[n_linea].orquestacion[i].canal[j] = linea.data()[0];
				linea = linea.substr( 2 );
			}

		}
		
		// arreglo inicial
		catalogo[n_linea].arreglo_inicial = std::stoi( linea );

		++n_linea;
	}

	log.close();
	archivo.close();

	// Cloning
	for ( int32_t i = 0; i < activeRows; ++i ) {
		strcpy( base[i].title,		catalogo[i].titulo.c_str() );
		strcpy( base[i].artist,		catalogo[i].artista.c_str() );
		strcpy( base[i].genre,		catalogo[i].genero.c_str() );
		strcpy( base[i].section,	catalogo[i].mood.c_str() );
		strcpy( base[i].keywords,	catalogo[i].key_words.c_str() );
		strcpy( base[i].type,		catalogo[i].tipo.c_str() );
		base[i].bnk = base[i].bnk;
		base[i].num = base[i].num;
	}
}

void Database::escribir( const std::string &_Path ) noexcept
{
	std::ofstream archivo{ _Path };

	if ( archivo.fail() ) {
		std::cerr << "No s pudo abrir " + _Path + "en BaseDeDatos::cargar()" << std::endl;
		exit( EXIT_FAILURE );
	}

	for ( int32_t i = 0; i < activeRows; ++i ) {
		// Compatibles
		catalogo[i].titulo		= base[i].title;
		catalogo[i].artista		= base[i].artist;
		catalogo[i].genero		= base[i].genre;
		catalogo[i].mood		= base[i].section;
		catalogo[i].key_words 	= base[i].keywords;
		catalogo[i].tipo 		= base[i].type;
		catalogo[i].bnk 		= base[i].bnk;
		catalogo[i].num 		= base[i].num;

		// Extra
		catalogo[i].n_arreglos = 2;

		catalogo[i].orquestacion[0].etiqueta = "Piano";
		for ( int32_t k = 0; k < 16; ++k )
				catalogo[i].orquestacion[0].canal[k] = k < 12 ? Switch::ON : Switch::OFF;

		catalogo[i].orquestacion[1].etiqueta = "Trompitas";
		for ( int32_t k = 0; k < 16; ++k )
				catalogo[i].orquestacion[1].canal[k] = k < 8 or 11 < k ? Switch::ON : Switch:: OFF;

		catalogo[i].arreglo_inicial = 0;

	}
	std::string delimitador;

	for ( int32_t i = 0; i < activeRows; ++i ) {
		delimitador = catalogo[i].titulo.find( ',' ) < catalogo[i].titulo.npos ? "\"" : "";
		archivo << delimitador << catalogo[i].titulo << delimitador << ',';

		delimitador = catalogo[i].artista.find( ',' ) < catalogo[i].artista.npos ? "\"" : "";
		archivo << delimitador << catalogo[i].artista << delimitador << ',';

		delimitador = catalogo[i].genero.find( ',' ) < catalogo[i].genero.npos ? "\"" : "";
		archivo << delimitador << catalogo[i].genero << delimitador << ',';

		archivo << catalogo[i].mood << ",";

		delimitador = catalogo[i].key_words.find( ',' ) < catalogo[i].key_words.npos ? "\"" : "";
		archivo << delimitador << base[i].keywords << delimitador << ",";

		archivo	<< catalogo[i].tipo	<< ","
				<< base[i].bnk	<< ","
				<< std::setw( 3 ) << std::setfill( '0' ) << base[i].num	<< ","

				<< catalogo[i].n_arreglos << ",";
		for ( int32_t j = 0; j < catalogo[i].n_arreglos; ++j ) {
				archivo << catalogo[i].orquestacion[j].etiqueta << ",";
				for ( int32_t k = 0; k < 8; ++k )
						archivo << catalogo[i].orquestacion[j].canal[k] << ",";
		}
		archivo << catalogo[i].arreglo_inicial << '\n';
	}

	archivo.close();
}

void Database::add_value(System row)
{
	base[activeRows++] = row;
}

void Database::edit_value(int line, System row)
{
	base[line] = row;
}

void Database::delete_value(int line)
{
	for (int i = line; i < activeRows - 1; i++)
		base[i] = base[i + 1];

	clean_row(activeRows--);
}

void Database::ordenate()
{
	int32_t a, b;
	int32_t k;
	bool success;
	System aux;

	//Sounds	
	a = 0;
	b = a + 1;
	while (a <= activeRows - 2 && b <= activeRows - 1) {	
		if (strstr(base[a].section, "Sound") == NULL || strstr("Sound", base[a].section) == NULL) {
			b = a + 1;
			success = false;
			while (success == false && b <= activeRows - 1) {
				if (strstr(base[b].section, "Sound") == NULL || strstr("Sound", base[b].section) == NULL)
					b++;
				else {
					aux = base[a];
					base[a] = base[b];
					base[b] = aux;
					success = true;
				}
			}
		}
		a++;
	}

	//Lobby
	a--;
	b = a + 1;
	while (a <= activeRows - 2 && b <= activeRows - 1) {	
		if (strstr(base[a].section, "Lobby") == NULL || strstr("Lobby", base[a].section) == NULL) {
			b = a + 1;
			success = false;
			while (success == false && b <= activeRows - 1) {
				if (strstr(base[b].section, "Lobby") == NULL || strstr("Lobby", base[b].section) == NULL)
					b++;
				else {
					aux = base[a];
					base[a] = base[b];
					base[b] = aux;
					success = true;
				}
			}
		}
		a++;
	}

	//Cena
	a--;
	b = a + 1;
	while (a <= activeRows - 2 && b <= activeRows - 1) {	
		if (strstr(base[a].section, "Cena") == NULL || strstr("Cena", base[a].section) == NULL) {
			b = a + 1;
			success = false;
			while (success == false && b <= activeRows - 1) {
				if (strstr(base[b].section, "Cena") == NULL || strstr("Cena", base[b].section) == NULL)
					b++;
				else {
					aux = base[a];
					base[a] = base[b];
					base[b] = aux;
					success = true;
				}
			}
		}
		a++;
	}

	//Baile
	a--;
	b = a + 1;
	while (a <= activeRows - 2 && b <= activeRows - 1) {	
		if (strstr(base[a].section, "Baile") == NULL || strstr("Baile", base[a].section) == NULL) {
			b = a + 1;
			success = false;
			while (success == false && b <= activeRows - 1) {
				if (strstr(base[b].section, "Baile") == NULL || strstr("Baile", base[b].section) == NULL)
					b++;
				else {
					aux = base[a];
					base[a] = base[b];
					base[b] = aux;
					success = true;
				}
			}
		}
		a++;
	}

	//ALphabet
	for (a = 0; a <= activeRows - 2; a++)
		for (b = a + 1; b <= activeRows - 1; b++) {
			if (strstr(base[a].section, base[b].section) &&
				strstr(base[b].section, base[a].section))  {
				k = 0;
				success = false;
				while (success == false && k < LONG_STRING) {
					if (base[a].title[k] > base[b].title[k]) { // cambio y EXITO
						aux 	= base[a];
						base[a] = base[b];
						base[b] = aux;
						success = true;		
					} else if (base[a].title[k] < base[b].title[k]) // se queda pero EXITO
						success = true;
						
					k++;
				}
			}
		}
}
