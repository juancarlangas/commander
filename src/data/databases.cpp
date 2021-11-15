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

Database::Database() : n_canciones( 0 )
{
	activeRows = n_canciones;
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

	n_canciones = 0;
	std::string linea;

	int32_t n_linea = 0;
	while ( std::getline( archivo, linea ) or linea != "" ) {
		++n_canciones;

		// Titulo
		if ( linea.starts_with( '\"' ) ) { // Incluye comas
			linea = linea.substr( 1 ); // Quitamos "
			cancion[n_linea].titulo = linea.substr( 0, linea.find_first_of( '\"' ) );
			linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
		}
		else {
			cancion[n_linea].titulo = linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 );
		}

		// Artista
		if ( linea.starts_with( '\"' ) ) { // Incluye comas
			linea = linea.substr( 1 ); // Quitamos "
			cancion[n_linea].artista = linea.substr( 0, linea.find_first_of( '\"' ) );
			linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
		}
		else {
			cancion[n_linea].artista = linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 );
		}

		// Genero
		if ( linea.starts_with( '\"' ) ) { // Incluye comas
			linea = linea.substr( 1 ); // Quitamos "
			cancion[n_linea].genero = linea.substr( 0, linea.find_first_of( '\"' ) );
			linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
		}
		else {
			cancion[n_linea].genero = linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 );
		}

		// Mood
		if ( linea.starts_with( '\"' ) ) { // Incluye comas
			linea = linea.substr( 1 ); // Quitamos "
			cancion[n_linea].mood = linea.substr( 0, linea.find_first_of( '\"' ) );
			linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
		}
		else {
			cancion[n_linea].mood = linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 );
		}

		// Keywords
		if ( linea.starts_with( '\"' ) ) { // Incluye comas
			linea = linea.substr( 1 ); // Quitamos "
			cancion[n_linea].keywords = linea.substr( 0, linea.find_first_of( '\"' ) );
			linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
		}
		else {
			cancion[n_linea].keywords = linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 );
		}

		// Tipo
		cancion[n_linea].tipo = linea.substr( 0, linea.find_first_of( ',' ) );
		linea = linea.substr( linea.find_first_of( ',' ) + 1 );

		// BNK
		base[n_linea].bnk = linea.data()[0];
		linea = linea.substr( 2 ); // garantiza que queda después de la 'coma'

		// NUM
		base[n_linea].num = std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) );
		linea = linea.substr( linea.find_first_of( ',' ) + 1 );

		// n_variaciones
		cancion[n_linea].n_variaciones = std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) );
		linea = linea.substr( 2 );

		///////////////////////////// VARIACIONES ///////////////////////////////////////
		for ( int32_t i = 0; i < cancion[n_linea].n_variaciones; ++i ) {

			// Label
			cancion[n_linea].variacion[i].etiqueta =
				linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 ); // 1 después de la 'coma'

			for ( int32_t j = 0; j < 8; ++j ) {
				// status
				cancion[n_linea].variacion[i].track[j].status =
					static_cast<enum Switch>(
							std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) ) );
				linea = linea.substr( linea.find_first_of( ',' ) + 1 ); // 1 después de la 'coma'

				// lower_key
				cancion[n_linea].variacion[i].track[j].lower_key =
					std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) );
				linea = linea.substr( linea.find_first_of( ',' ) + 1 ); // 1 después de la 'coma'

				// upper_key
				cancion[n_linea].variacion[i].track[j].upper_key =
					std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) );
				linea = linea.substr( linea.find_first_of( ',' ) + 1 ); // 1 después de la 'coma'

				// transposition
				cancion[n_linea].variacion[i].track[j].transposition =
					std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) );
				linea = linea.substr( linea.find_first_of( ',' ) + 1 ); // 1 después de la 'coma'
			}
		}
		
		// arreglo inicial
		cancion[n_linea].variacion_inicial = std::stoi( linea.substr( 0, linea.find( ',' ) ) );

		++n_linea;
	}

	log.close();
	archivo.close();

	// Cloning
	activeRows = n_canciones;
	for ( int32_t i = 0; i < activeRows; ++i ) {
		strcpy( base[i].title,		cancion[i].titulo.c_str() );
		strcpy( base[i].artist,		cancion[i].artista.c_str() );
		strcpy( base[i].genre,		cancion[i].genero.c_str() );
		strcpy( base[i].section,	cancion[i].mood.c_str() );
		strcpy( base[i].keywords,	cancion[i].keywords.c_str() );
		strcpy( base[i].type,		cancion[i].tipo.c_str() );
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

	n_canciones = activeRows;

	for ( int32_t i = 0; i < n_canciones; ++i ) {
		// Compatibles
		cancion[i].titulo	= base[i].title;
		cancion[i].artista	= base[i].artist;
		cancion[i].genero	= base[i].genre;
		cancion[i].mood		= base[i].section;
		cancion[i].keywords = base[i].keywords;
		cancion[i].tipo 	= base[i].type;
		cancion[i].bnk 		= base[i].bnk;
		cancion[i].num 		= base[i].num;

		// Extra
		cancion[i].n_variaciones = 2;

		cancion[i].variacion[0].etiqueta = "Piano";
		for ( int32_t k = 0; k < 8; ++k ) {
			cancion[i].variacion[0].track[k].status = k < 5 ? Switch::ON : Switch::OFF;
			cancion[i].variacion[0].track[k].lower_key = 0;
			cancion[i].variacion[0].track[k].upper_key = 127;
			cancion[i].variacion[0].track[k].transposition = 0;
		}

		cancion[i].variacion[1].etiqueta = "Trompitas";
		for ( int32_t k = 0; k < 8; ++k ) {
			cancion[i].variacion[1].track[k].status = k < 2 or 5 <= k ? Switch::ON : Switch::OFF;
			cancion[i].variacion[1].track[k].lower_key = 0;
			cancion[i].variacion[1].track[k].upper_key = 127;
			cancion[i].variacion[1].track[k].transposition = 0;
		}

		cancion[i].variacion_inicial = 0;

	}

	std::string delimitador;

	for ( int32_t i = 0; i < activeRows; ++i ) {
		delimitador = cancion[i].titulo.find( ',' ) < cancion[i].titulo.npos ? "\"" : "";
		archivo << delimitador << cancion[i].titulo << delimitador << ',';

		delimitador = cancion[i].artista.find( ',' ) < cancion[i].artista.npos ? "\"" : "";
		archivo << delimitador << cancion[i].artista << delimitador << ',';

		delimitador = cancion[i].genero.find( ',' ) < cancion[i].genero.npos ? "\"" : "";
		archivo << delimitador << cancion[i].genero << delimitador << ',';

		archivo << cancion[i].mood << ",";

		delimitador = cancion[i].keywords.find( ',' ) < cancion[i].keywords.npos ? "\"" : "";
		archivo << delimitador << base[i].keywords << delimitador << ",";

		archivo	<< cancion[i].tipo	<< ","
				<< base[i].bnk	<< ","
				<< std::setw( 3 ) << std::setfill( '0' ) << base[i].num	<< ","

				<< cancion[i].n_variaciones << ",";
		for ( int32_t j = 0; j < cancion[i].n_variaciones; ++j ) {
			archivo << cancion[i].variacion[j].etiqueta << ",";
			for ( int32_t k = 0; k < 8; ++k ) {
					archivo << cancion[i].variacion[j].track[k].status << ",";
					archivo << cancion[i].variacion[j].track[k].lower_key << ",";
					archivo << cancion[i].variacion[j].track[k].upper_key << ",";
					archivo << cancion[i].variacion[j].track[k].transposition << ",";
			}
		}
		archivo << cancion[i].variacion_inicial << '\n';
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
