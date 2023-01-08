#include <bits/stdint-intn.h>
#include <cstdlib>
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

#include "data/databases.hpp"
#include "graphics/form.hpp"
#include "common/string.hpp"
#include "utilities/src/files.hpp"

Database::Database() :/*{{{*/
		activeRows( 0 ),
		n_canciones( 0 )
{
	// homedir
	if ((homedir = getenv("HOME")) == NULL)
		homedir = getpwuid(getuid())->pw_dir;
}/*}}}*/

Database::Database( const std::string &_Path ) noexcept/*{{{*/
{
	// homedir
	if ((homedir = getenv("HOME")) == NULL)
		homedir = getpwuid(getuid())->pw_dir;

	cargar( _Path );
}/*}}}*/

Database::Database( const std::string &_Path, Combinations *_CombinationsPtr ) noexcept :/*{{{*/
	combinationsPtr( _CombinationsPtr )
{
	// homedir
	if ((homedir = getenv("HOME")) == NULL)
		homedir = getpwuid(getuid())->pw_dir;

	cargar( _Path );
}/*}}}*/

void Database::clean_row(int line)/*{{{*/
{
	*base[line].title = '\0';
	*base[line].artist = '\0';
	*base[line].genre = '\0';
	*base[line].section = '\0';
	*base[line].keywords = '\0';
	*base[line].type = '\0';
	base[line].bnk = 0;
	base[line].num = 0;
}/*}}}*/

void Database::cargar( const std::string &_Path ) noexcept/*{{{*/
{
	activeRows = n_canciones = Files::contar_lineas( _Path );

	std::ifstream archivo{ _Path };
	if ( archivo.fail() ) {
		std::cerr << "No se pudo abrir " + _Path + "en BaseDeDatos::cargar()" << std::endl;
		exit( EXIT_FAILURE );
	}

	for ( int32_t i = 0; i < n_canciones; i++ )
		clean_row(i);

	std::string linea;

	// Leemos
	for ( int32_t n_linea = 0; n_linea < n_canciones; ++n_linea ) {
		// Titulo
		std::getline( archivo, linea );
		if ( linea.starts_with( '\"' ) ) { // Incluye comas
			linea = linea.substr( 1 ); // Quitamos "
			base[n_linea].titulo = linea.substr( 0, linea.find_first_of( '\"' ) );
			linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
		}
		else {
			base[n_linea].titulo = linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 );
		}

		// Artista
		if ( linea.starts_with( '\"' ) ) { // Incluye comas
			linea = linea.substr( 1 ); // Quitamos "
			base[n_linea].artista = linea.substr( 0, linea.find_first_of( '\"' ) );
			linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
		}
		else {
			base[n_linea].artista = linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 );
		}

		// Genero
		if ( linea.starts_with( '\"' ) ) { // Incluye comas
			linea = linea.substr( 1 ); // Quitamos "
			base[n_linea].genero = linea.substr( 0, linea.find_first_of( '\"' ) );
			linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
		}
		else {
			base[n_linea].genero = linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 );
		}

		// Mood
		if ( linea.starts_with( '\"' ) ) { // Incluye comas
			linea = linea.substr( 1 ); // Quitamos "
			base[n_linea].mood = linea.substr( 0, linea.find_first_of( '\"' ) );
			linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
		}
		else {
			base[n_linea].mood = linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 );
		}

		// Keywords
		if ( linea.starts_with( '\"' ) ) { // Incluye comas
			linea = linea.substr( 1 ); // Quitamos "
			base[n_linea].key_words = linea.substr( 0, linea.find_first_of( '\"' ) );
			linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
		}
		else {
			base[n_linea].key_words = linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 );
		}

		// Tipo
		base[n_linea].tipo = linea.substr( 0, linea.find_first_of( ',' ) );
		linea = linea.substr( linea.find_first_of( ',' ) + 1 );

		// BNK
		base[n_linea].bnk = linea.data()[0];
		linea = linea.substr( 2 ); // garantiza que queda después de la 'coma'

		// NUM
		base[n_linea].num = std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) );
		linea = linea.substr( linea.find_first_of( ',' ) + 1 );

		///////////////////////////// VARIACIONES ///////////////////////////////////////
		// n_variaciones
		base[n_linea].n_variaciones = std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) );
		if ( base[n_linea].n_variaciones > MAX_VARIATIONS ) {
			std::cerr << "Exceso de variaciones en Database::cargar(), línea " << n_linea
				<< std::endl;
			exit( EXIT_FAILURE );
		}
		linea = linea.substr( linea.find_first_of( ',' ) + 1 ); // 1 después de la 'coma'

		// Variación inicial
		base[n_linea].variacion_inicial = std::stoi( linea.substr( 0, linea.find( ',' ) ) );
		linea = linea.substr( linea.find_first_of( ',' ) + 1 ); // 1 después de la 'coma'


		// Label
		for ( int32_t i = 0; i < base[n_linea].n_variaciones; ++i ) {
			base[n_linea].variacion[i].etiqueta =
				linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 ); // 1 después de la 'coma'
		}

		for ( int32_t i = 0; i < 8; ++i ) {

			// instrumento
			if ( linea.starts_with( '\"' ) ) { // Incluye comas
				linea = linea.substr( 1 ); // Quitamos "
				base[n_linea].instrumento[ i ] = linea.substr( 0, linea.find_first_of( '\"' ) );
				linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
			}
			else {
				base[n_linea].instrumento[ i ] = linea.substr( 0, linea.find_first_of( ',' ) );
				linea = linea.substr( linea.find_first_of( ',' ) + 1 );
			}

			for ( int32_t j = 0; j < base[n_linea].n_variaciones; ++j ) {
				// status
				base[n_linea].variacion[j].track[i].status = static_cast<enum Switch>(
							std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) ) );
				linea = linea.substr( linea.find_first_of( ',' ) + 1 ); // 1 después de la 'coma'

				// volume
				base[n_linea].variacion[j].track[i].volume =
					std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) );
				linea = linea.substr( linea.find_first_of( ',' ) + 1 ); // 1 después de la 'coma'

				// lower_key
				base[n_linea].variacion[j].track[i].lower_key =
					std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) );
				linea = linea.substr( linea.find_first_of( ',' ) + 1 ); // 1 después de la 'coma'

				// upper_key
				base[n_linea].variacion[j].track[i].upper_key =
					std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) );
				linea = linea.substr( linea.find_first_of( ',' ) + 1 ); // 1 después de la 'coma'

				// transposition
				base[n_linea].variacion[j].track[i].transposition =
					std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) );
				linea = linea.substr( linea.find_first_of( ',' ) + 1 ); // 1 después de la 'coma'
			}
		}
		
	}

	archivo.close();

	// Cloning
	activeRows = n_canciones;
	for ( int32_t i = 0; i < activeRows; ++i ) {
		strcpy( base[i].title,		base[i].titulo.c_str() );
		strcpy( base[i].artist,		base[i].artista.c_str() );
		strcpy( base[i].genre,		base[i].genero.c_str() );
		strcpy( base[i].section,	base[i].mood.c_str() );
		strcpy( base[i].keywords,	base[i].key_words.c_str() );
		strcpy( base[i].type,		base[i].tipo.c_str() );
	}

	delete_duplicated();

	// Favoritos{{{
	int32_t i { 0 };
	int16_t favoritos_leidos { 0 };
	
	while ( i < n_canciones and favoritos_leidos < N_FAVORITOS ) {
		if ( base[ i ].key_words.substr( 0, 10 ) == "Favourite_" ) {
			favorito[ std::stoi( base[ i ].key_words.substr( 10 ) ) - 1 ] = base + i;
			++favoritos_leidos;
		}
		++i;
	}

	if ( i == n_canciones ) {
		std::cerr << "Database::cargar() no encontró el número suficiente de favoritos"
			<< std::endl;
		exit( EXIT_FAILURE );
	}/*}}}*/
}/*}}}*/

void Database::cargar_especifico( const std::string &_Path, int32_t _Indice ) noexcept/*{{{*/
{
	std::ifstream archivo{ _Path };
	if ( archivo.fail() ) {
		std::cerr << "No se pudo abrir " + _Path + "en BaseDeDatos::cargar()" << std::endl;
		exit( EXIT_FAILURE );
	}

	std::string linea;
	for ( int32_t i = 0; i < _Indice; ++i )
		std::getline( archivo, linea );

	std::getline( archivo, linea ); // cargamos la definitiva

	if ( linea.starts_with( '\"' ) ) { // Incluye comas
		linea = linea.substr( 1 ); // Quitamos "
		base[ _Indice ].titulo = linea.substr( 0, linea.find_first_of( '\"' ) );
		linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
	}
	else {
		base[ _Indice ].titulo = linea.substr( 0, linea.find_first_of( ',' ) );
		linea = linea.substr( linea.find_first_of( ',' ) + 1 );
	}

	// Artista
	if ( linea.starts_with( '\"' ) ) { // Incluye comas
		linea = linea.substr( 1 ); // Quitamos "
		base[ _Indice ].artista = linea.substr( 0, linea.find_first_of( '\"' ) );
		linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
	}
	else {
		base[ _Indice ].artista = linea.substr( 0, linea.find_first_of( ',' ) );
		linea = linea.substr( linea.find_first_of( ',' ) + 1 );
	}

	// Genero
	if ( linea.starts_with( '\"' ) ) { // Incluye comas
		linea = linea.substr( 1 ); // Quitamos "
		base[ _Indice ].genero = linea.substr( 0, linea.find_first_of( '\"' ) );
		linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
	}
	else {
		base[ _Indice ].genero = linea.substr( 0, linea.find_first_of( ',' ) );
		linea = linea.substr( linea.find_first_of( ',' ) + 1 );
	}

	// Mood
	if ( linea.starts_with( '\"' ) ) { // Incluye comas
		linea = linea.substr( 1 ); // Quitamos "
		base[ _Indice ].mood = linea.substr( 0, linea.find_first_of( '\"' ) );
		linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
	}
	else {
		base[ _Indice ].mood = linea.substr( 0, linea.find_first_of( ',' ) );
		linea = linea.substr( linea.find_first_of( ',' ) + 1 );
	}

	// Keywords
	if ( linea.starts_with( '\"' ) ) { // Incluye comas
		linea = linea.substr( 1 ); // Quitamos "
		base[ _Indice ].key_words = linea.substr( 0, linea.find_first_of( '\"' ) );
		linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
	}
	else {
		base[ _Indice ].key_words = linea.substr( 0, linea.find_first_of( ',' ) );
		linea = linea.substr( linea.find_first_of( ',' ) + 1 );
	}

	// Tipo
	base[ _Indice ].tipo = linea.substr( 0, linea.find_first_of( ',' ) );
	linea = linea.substr( linea.find_first_of( ',' ) + 1 );

	// BNK
	base[ _Indice ].bnk = linea.data()[0];
	linea = linea.substr( 2 ); // garantiza que queda después de la 'coma'

	// NUM
	base[ _Indice ].num = std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) );
	linea = linea.substr( linea.find_first_of( ',' ) + 1 );

	///////////////////////////// VARIACIONES ///////////////////////////////////////
	// n_variaciones
	base[ _Indice ].n_variaciones = std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) );
	linea = linea.substr( linea.find_first_of( ',' ) + 1 ); // 1 después de la 'coma'

	// Label
	for ( int32_t i = 0; i < base[ _Indice ].n_variaciones; ++i ) {
		base[ _Indice ].variacion[i].etiqueta =
			linea.substr( 0, linea.find_first_of( ',' ) );
		linea = linea.substr( linea.find_first_of( ',' ) + 1 ); // 1 después de la 'coma'
	}

	// Variación inicial
	base[ _Indice ].variacion_inicial = std::stoi( linea.substr( 0, linea.find( ',' ) ) );
	linea = linea.substr( linea.find_first_of( ',' ) + 1 ); // 1 después de la 'coma'

	for ( int32_t i = 0; i < 8; ++i ) {
		// instrumento
		if ( linea.starts_with( '\"' ) ) { // Incluye comas
			linea = linea.substr( 1 ); // Quitamos "
			base[ _Indice ].instrumento[ i ] = linea.substr( 0, linea.find_first_of( '\"' ) );
			linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
		}
		else {
			base[ _Indice ].instrumento[ i ] = linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 );
		}

		for ( int32_t j = 0; j < base[ _Indice ].n_variaciones; ++j ) {
			// status
			base[ _Indice ].variacion[j].track[i].status = static_cast<enum Switch>(
						std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 ); // 1 después de la 'coma'

			// lower_key
			base[ _Indice ].variacion[j].track[i].lower_key =
				std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 ); // 1 después de la 'coma'

			// upper_key
			base[ _Indice ].variacion[j].track[i].upper_key =
				std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 ); // 1 después de la 'coma'

			// transposition
			base[ _Indice ].variacion[j].track[i].transposition =
				std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) );
				linea = linea.substr( linea.find_first_of( ',' ) + 1 ); // 1 después de la 'coma'
			}
	}
}/*}}}*/

int32_t Database::get_activeRows() noexcept/*{{{*/
{
	return n_canciones;
}/*}}}*/

void Database::escribir( const std::string &_Path ) noexcept/*{{{*/
{
	std::ofstream archivo{ _Path };

	if ( archivo.fail() ) {
		std::cerr << "No s pudo abrir " + _Path + "en BaseDeDatos::cargar()" << std::endl;
		exit( EXIT_FAILURE );
	}

	////////////////////////////////// clonando del esquema C ///////////////////////////////////
	n_canciones = activeRows;

	for ( int32_t i = 0; i < n_canciones; ++i ) {
		base[i].titulo		= base[i].title;
		base[i].artista		= base[i].artist;
		base[i].genero		= base[i].genre;
		base[i].mood		= base[i].section;
		base[i].key_words	= base[i].keywords;
		base[i].tipo		= base[i].type;
	}

	///////////////////////////////////// escritura ///////////////////////////////////////////
	std::string delimitador;

	for ( int32_t i = 0; i < activeRows; ++i ) {
		// Tags
		delimitador = base[i].titulo.find( ',' ) < base[i].titulo.npos ? "\"" : "";
		archivo << delimitador << base[i].titulo << delimitador << ',';

		delimitador = base[i].artista.find( ',' ) < base[i].artista.npos ? "\"" : "";
		archivo << delimitador << base[i].artista << delimitador << ',';

		delimitador = base[i].genero.find( ',' ) < base[i].genero.npos ? "\"" : "";
		archivo << delimitador << base[i].genero << delimitador << ',';

		archivo << base[i].mood << ',';

		delimitador = base[i].key_words.find( ',' ) < base[i].key_words.npos ? "\"" : "";
		archivo << delimitador << base[i].key_words << delimitador << ",";

		archivo	<< base[i].tipo	<< ','
				<< base[i].bnk	<< ','
				<< std::setw( 3 ) << std::setfill( '0' ) << base[i].num	<< ","
				<< base[i].n_variaciones << ','
				<< base[i].variacion_inicial << ',';

		// Etiqueta de variación
		for ( int32_t j = 0; j < base[i].n_variaciones; ++j ) {
			delimitador = 	base[i].variacion[j].etiqueta.find( ',' ) <
							base[i].variacion[j].etiqueta.npos	? "\"" : "";
			archivo << delimitador << base[i].variacion[j].etiqueta << delimitador << ',';
		}

		for ( int32_t k = 0; k < 8; ++k ) {
			delimitador = base[i].instrumento[ k ].find( ',' ) < base[i].instrumento[ k ].npos ?
				"\"" : "";
			archivo << delimitador << base[i].instrumento[ k ] << delimitador << ',';
			for ( int32_t j = 0; j < base[i].n_variaciones; ++j ) {
				archivo << base[i].variacion[j].track[k].status << ','
						<< base[i].variacion[j].track[k].volume << ','
						<< base[i].variacion[j].track[k].lower_key << ','
						<< base[i].variacion[j].track[k].upper_key << ','
						<< base[i].variacion[j].track[k].transposition
						<< ( k < 7 or j < base[i].n_variaciones - 1 ? ',' : '\n' );
			}
		}
	}

	archivo.close();
}/*}}}*/

void Database::add_value( System row )/*{{{*/
{
	base[activeRows] = row;

	// C++ values
	base[activeRows].titulo = base[activeRows].title;
	base[activeRows].artista = base[activeRows].artist;
	base[activeRows].genero = base[activeRows].genre;
	base[activeRows].mood = base[activeRows].section;
	base[activeRows].key_words = base[activeRows].keywords;
	base[activeRows].tipo = base[activeRows].type;

	// extras
	base[activeRows].n_variaciones = 1;
	base[activeRows].variacion_inicial = 0;

	base[activeRows].variacion[0].etiqueta = "Label";

	for ( int32_t i = 0; i < 8; ++i ) {
		base[activeRows].instrumento[i] =
			combinationsPtr->get_instrument_name( row.bnk, row.num, i );
		base[activeRows].variacion[0].track[i].status = Switch::OFF;
		base[activeRows].variacion[0].track[i].volume = 100;
		base[activeRows].variacion[0].track[i].transposition = 0;
		base[activeRows].variacion[0].track[i].lower_key = 36;
		base[activeRows].variacion[0].track[i].upper_key = 96;
	}

	n_canciones = ++activeRows;
}/*}}}*/

void Database::edit_value(int line, System row)/*{{{*/
{
	base[line] = row;
}/*}}}*/

void Database::delete_value( int line )/*{{{*/
{
	for ( int i = line; i < activeRows - 1; i++ )
		base[i] = base[i + 1];

	clean_row( activeRows-- );
	n_canciones = activeRows;
}/*}}}*/

void Database::ordenate()/*{{{*/
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
}/*}}}*/

void Database::delete_duplicated() noexcept/*{{{*/
{
	int32_t i, j;
	for ( i = 0; i < n_canciones - 1; ++i )
		for ( j = i + 1; j < n_canciones; ++j )
			if ( ( base[i].titulo == base[j].titulo ) and ( base[i].artista == base[j].artista ) ) {
				delete_value( j );
				--n_canciones;
			}
}/*}}}*/

struct System Database::get_cancion( const int _Index ) noexcept/*{{{*/
{
	return base[ _Index ];
}/*}}}*/

struct System *Database::get_cancion_ptr( const int32_t &_Index ) noexcept/*{{{*/
{
	return base + _Index;
}/*}}}*/

struct System *Database::get_favourite_row( const int32_t &_FavNumber ) noexcept/*{{{*/
{
	return favorito[ _FavNumber ];
}/*}}}*/
