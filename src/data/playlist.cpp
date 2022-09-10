#include "playlist.hpp"
#include "databases.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>

// Inicializa el puntero a la Base de datos e invoca cargar()
Playlist::Playlist( const std::string &_Path, Database *_Database_ptr )/*{{{*/
{
	database_ptr = _Database_ptr;
	cargar( _Path );
}/*}}}*/

void Playlist::cargar( const std::string &_Path ) noexcept/*{{{*/
{
	/* Lee línea por línea del archivo almacenando en un pequeño struct temporal
	 * y luego revisa que esa combinación se encuentre en la base de datos;
	 * si no se encuentra descarta dicha línea, de lo contrario la agrega al playlist */

	std::ifstream archivo { _Path };
	if ( archivo.fail() ) {
		std::cerr << "No se pudo abrir " << _Path << " en contar_lineas()" << std::endl;
		exit( EXIT_FAILURE );
	}

	std::string linea;
	struct {
		std::string titulo;
		std::string artista;
	} pista_temp;

	n_pistas = 0;

	while( std::getline( archivo, linea ) ) {

		// Titulo
		if ( linea.starts_with( '\"' ) ) { // si incluye comillas
			linea = linea.substr( 1 ); // avanzamos 1
			pista[n_pistas].titulo = linea.substr( 0, linea.find_first_of( '\"' ) );
			linea = linea.substr( linea.find_first_of( '\"' ) + 2 ); // lo dejamos hasta el sig
		}
		else {
			pista[n_pistas].titulo = linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 );
		}

		// Artista
		if ( linea.starts_with( '\"' ) ) { // Incluye comas
			linea = linea.substr( 1 ); // Quitamos "
			pista[n_pistas].artista = linea.substr( 0, linea.find_first_of( '\"' ) );
			linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
		}
		else {
			pista[n_pistas].artista = linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 );
		}

		++n_pistas;
	}

	archivo.close();
	sincronizar();
}/*}}}*/

void Playlist::agregar( struct System * const &_Cancion ) noexcept/*{{{*/
{
	pista[ n_pistas ].titulo = _Cancion->titulo;
	pista[ n_pistas ].artista = _Cancion->artista;
	pista[ n_pistas ].row_ptr = _Cancion;

	++n_pistas;
}/*}}}*/

void Playlist::eliminar( const int32_t &_Index ) noexcept/*{{{*/
{
	for ( int32_t i = _Index; i < n_pistas - 1; ++i ) // succión
		pista[ i ] = pista[ i + 1 ];

	--n_pistas;
}/*}}}*/

void Playlist::guardar( const std::string &_Path) noexcept/*{{{*/
{
	std::ofstream archivo{ _Path };

	if ( archivo.fail() ) {
		std::cerr << "No s pudo abrir " + _Path + "en BaseDeDatos::cargar()" << std::endl;
		exit( EXIT_FAILURE );
	}

	///////////////////////////////////// escritura ///////////////////////////////////////////
	std::string delimitador;

	for ( int32_t i = 0; i < n_pistas; ++i ) {
		// Tags
		delimitador = pista[i].titulo.find( ',' ) < pista[i].titulo.npos ? "\"" : "";
		archivo << delimitador << pista[i].titulo << delimitador << ',';

		delimitador = pista[i].artista.find( ',' ) < pista[i].artista.npos ? "\"" : "";
		archivo << delimitador << pista[i].artista << delimitador << ',';
	}

	archivo.close();
}/*}}}*/

void Playlist::sincronizar() noexcept/*{{{*/
{
	int32_t i, j;

	for ( i = 0; i < n_pistas; ++i ) {
		j = 0;
		while ( j < database_ptr->get_activeRows() and
				pista[i].titulo != database_ptr->get_cancion(j).titulo and
				pista[i].artista != database_ptr->get_cancion(j).artista )
			++j;
		if ( j < database_ptr->get_activeRows() ) // lo encontró -> anéxale el (nuevo) apuntador
			pista[i].row_ptr = database_ptr->get_cancion_ptr(j);
		else // elmimínala del playlist
			eliminar( i );
	}
}/*}}}*/

int32_t Playlist::get_n_pistas() noexcept/*{{{*/
{
	return n_pistas;
}/*}}}*/

const std::string &Playlist::get_titulo( const int32_t &_Index ) noexcept/*{{{*/
{
	return pista[ _Index ].titulo;
}/*}}}*/

const std::string &Playlist::get_artista( const int32_t &_Index ) noexcept/*{{{*/
{
	return pista[ _Index ].titulo;
}/*}}}*/
		
Playlist::~Playlist() {}
