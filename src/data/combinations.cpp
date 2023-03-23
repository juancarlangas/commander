#include "combinations.hpp"
#include <bits/stdint-intn.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "utilities/src/files.hpp"

int32_t value;

Combinations::Combinations( const std::string &_Path ) :/*{{{*/
	n_bancos { static_cast<int16_t>( ( value = Files::contar_lineas( _Path ) ) / 128 ) },
	data { new struct Row [ n_bancos * 128 ]() }
{
	std::ifstream archivo { _Path };
	if ( archivo.fail() ) {
		std::cerr << "No se pudo abrir " + _Path + "en BaseDeDatos::cargar()" << std::endl;
		exit( EXIT_FAILURE );
	}

	std::string linea;

	for ( int32_t i = 0; i < n_bancos * 128; ++i ) {
		std::getline( archivo, linea );

		// BNK
		data[i].bnk = linea.data()[0];
		linea = linea.substr( 2 ); // garantiza que queda después de la 'coma'

		// NUM
		data[i].num = std::stoi( linea.substr( 0, linea.find_first_of( ',' ) ) );
		linea = linea.substr( linea.find_first_of( ',' ) + 1 );

		// NOMBRE
		if ( linea.starts_with( '\"' ) ) { // Incluye comas
			data[i].nombre = linea.substr( 1, linea.find_first_of( '\"' ) );
			linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
		}
		else {
			data[i].nombre = linea.substr( 0, linea.find_first_of( ',' ) );
			linea = linea.substr( linea.find_first_of( ',' ) + 1 );
		}

		// INSTRUMENTOS
		for ( int32_t j = 0; j < 8; ++j ) {
			if ( linea.starts_with( '\"' ) ) {
				data[i].instrumento[j] = linea.substr( 1, linea.find_first_of( '\"' ) );
				linea = linea.substr( linea.find_first_of( '\"' ) + 2 );
			}
			else {
				data[i].instrumento[j] = linea.substr( 0, linea.find_first_of( ',' ) );
				linea = linea.substr( linea.find_first_of( ',' ) + 1 );
			}
		}
	}

	archivo.close();
}/*}}}*/

Combinations::~Combinations()/*{{{*/
{
	delete [] data;
}/*}}}*/

void Combinations::escribir( const std::string &_Path ) noexcept/*{{{*/
{
	std::ofstream archivo { _Path };
	if ( archivo.fail() ) {
		std::cerr << "No se pudo abrir " + _Path + "en BaseDeDatos::escribir()" << std::endl;
		exit( EXIT_FAILURE );
	}

	for ( int32_t i = 0; i < n_bancos * 128; ++i ) {
		archivo << data[i].bnk << ',' << std::setw( 3 ) << std::setfill( '0' ) << data[i].num << ','
			<< data[i].nombre;
		for ( int16_t j = 0; j < 8; ++j )
			archivo << ',' << data[i].instrumento[j];
		archivo << '\n';
	}
	
	archivo.close();
}/*}}}*/

std::string Combinations::get_instrument_name(/*{{{*/
		const char &_Banco, const int16_t &_Numero, const int16_t &_Track ) noexcept
{
	return data[ bnk_num_to_int( _Banco, _Numero ) ].instrumento[ _Track ];
}/*}}}*/

void Combinations::set_instrument_name(/*{{{*/
		const char &_Banco, const int16_t &_Numero, const int16_t &_Pista,
		const std::string_view _Nombre )
			noexcept
{
	data[ bnk_num_to_int( _Banco, _Numero ) ].instrumento[ _Pista ] = _Nombre;
}/*}}}*/

int16_t Combinations::bnk_num_to_int( const char &_Banco, const int16_t &_Numero ) noexcept/*{{{*/
{
	return ( _Banco - 65 ) * 127 + _Numero;
}/*}}}*/
