#include "combinations.hpp"

#include <bits/stdint-intn.h>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <stdexcept>
#include <string>
#include <algorithm>

#include "utilities/src/files.hpp"

int32_t value;

Combinations::Combinations( const std::string &_Path )/*{{{*/
{
	load_from_json( _Path );
}/*}}}*/

void Combinations::load_from_csv( const std::string &_Path ) noexcept/*{{{*/
{
	n_bancos = static_cast<int16_t>( ( value = Files::contar_lineas( _Path ) ) / 128 );
	data = new struct Row [ n_bancos * 128 ]();

	std::ifstream archivo { _Path };
	if ( archivo.fail() ) {
		std::cerr << "No se pudo abrir " + _Path + "en BaseDeDatos::cargar()" << std::endl;
		exit( EXIT_FAILURE );
	}

	std::string linea;

	for ( size_t i = 0; i < n_bancos * 128; ++i ) {
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

void Combinations::load_from_json( const std::string &_Path )/*{{{*/
{
	// LOAD DATA
	std::ifstream json_file{ _Path };
	if ( json_file.fail() ) {

		std::cerr << "Failed to open " + _Path + " in Combinations::load_from_json()\n";
		exit(EXIT_FAILURE);
	}
	nlohmann::json json_object;
	json_file >> json_object;
	json_object.get_to(combination_list);

	json_file.close();

	// CALCULATE SIZES
	n_bancos = combination_list.size();

	if ( !combination_list.empty() ) {
		channels_per_combi = combination_list[0][0].instrument_list.size();

		for ( const auto &bank : combination_list )
			for ( const auto &combination : bank )
				if ( combination.instrument_list.size() != channels_per_combi ) {
					std::cerr << "instrument_list vector size mismatch\n";
					exit( EXIT_FAILURE );
				}
	}
	else
		throw std::runtime_error( "combination_list is empty" );

}/*}}}*/

void Combinations::from_new_to_old() noexcept/*{{{*/
{
	data = new struct Row [ n_bancos * PATCHES_PER_BANK ]();

	for ( size_t i_bank{ 0 }; i_bank < n_bancos; ++i_bank )
		for ( size_t i_patch{ 0 }; i_patch < PATCHES_PER_BANK; ++i_patch ) {
			data[ i_bank * PATCHES_PER_BANK + i_patch ].bnk = std::to_string( i_bank ).c_str()[0];
			data[ i_bank * PATCHES_PER_BANK + i_patch ].num = i_patch;
			data[ i_bank * PATCHES_PER_BANK + i_patch ].nombre = combination_list[i_bank][i_patch].name;
			for ( size_t i_inst{ 0 }; i_inst < channels_per_combi; ++i_inst )
				data[ i_bank * PATCHES_PER_BANK + i_patch ].instrumento[i_inst] =
					combination_list[i_bank][i_patch].instrument_list[i_inst];
		}
}/*}}}*/

Combinations::~Combinations()/*{{{*/
{
	// delete [] data;
}/*}}}*/

void Combinations::escribir( const std::string &_Path ) noexcept/*{{{*/
{
	std::ofstream archivo { _Path };
	if ( archivo.fail() ) {
		std::cerr << "No se pudo abrir " + _Path + "en BaseDeDatos::escribir()" << std::endl;
		exit( EXIT_FAILURE );
	}

	for ( size_t i = 0; i < n_bancos * 128; ++i ) {
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

void from_json( const nlohmann::json &_JSONobject, struct Combi &_Combination ) {/*{{{*/
    _JSONobject.at( "name").get_to( _Combination.name );
    _JSONobject.at( "instrument_list").get_to( _Combination.instrument_list );
}/*}}}*/

