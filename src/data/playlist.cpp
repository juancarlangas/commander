#include "playlist.hpp"
#include "databases.hpp"
#include <cstdint>
#include <fstream>

Playlist::Playlist( const std::string &_Path )/*{{{*/
{
	cargar( _Path );
}/*}}}*/

void Playlist::cargar( const std::string &_Path ) noexcept/*{{{*/
{
	n_pistas = Files::contar_lineas( _Path );

	std::ifstream archivo { _Path };
	std::string linea;

	for ( int32_t n_linea = 0; n_linea < n_pistas; ++n_linea ) {
		// Titulo
		std::getline( archivo, linea );
		if ( linea.starts_with( '\"' ) ) { // Incluye comas
			linea = linea.substr( 1 ); // Quitamos "
			pista[n_linea].titulo = linea.substr( 0, linea.find_first_of( '\"' ) );
			linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
		}
		else {
			pista[n_linea].titulo = linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 );
		}

		// Artista
		if ( linea.starts_with( '\"' ) ) { // Incluye comas
			linea = linea.substr( 1 ); // Quitamos "
			pista[n_linea].artista = linea.substr( 0, linea.find_first_of( '\"' ) );
			linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
		}
		else {
			pista[n_linea].artista = linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 );
		}
	}
	archivo.close();
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
