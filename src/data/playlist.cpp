#include "playlist.hpp"
#include "databases.hpp"
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

Playlist::~Playlist() {}
