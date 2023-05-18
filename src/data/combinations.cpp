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
	json_object.get_to(combinations);

	json_file.close();

	// CALCULATE SIZES
	n_bancos = combinations.size();

	if ( !combinations.empty() ) {
		channels_per_combi = combinations[0][0].instruments.size();

		for ( const auto &bank : combinations )
			for ( const auto &combination : bank )
				if ( combination.instruments.size() != channels_per_combi ) {
					std::cerr << "instruments vector size mismatch\n";
					exit( EXIT_FAILURE );
				}
	}
	else
		throw std::runtime_error( "combinations is empty" );
}/*}}}*/

Combinations::~Combinations()/*{{{*/
{
	// delete [] data;
}/*}}}*/

std::string Combinations::get_instrument_name(/*{{{*/
		const char &_Banco, const int16_t &_Numero, const int16_t &_Track ) noexcept
{
	return combinations[_Banco][_Numero].instruments[ _Track ];
}/*}}}*/

void Combinations::set_instrument_name(/*{{{*/
		const char &_Banco, const int16_t &_Numero, const int16_t &_Pista,
		const std::string_view _Nombre )
			noexcept
{
	combinations[_Banco][_Numero].instruments[ _Pista ] = _Nombre;
}/*}}}*/

int16_t Combinations::bnk_num_to_int( const char &_Banco, const int16_t &_Numero ) noexcept/*{{{*/
{
	return ( _Banco - 65 ) * 127 + _Numero;
}/*}}}*/

void from_json( const nlohmann::json &_JSONobject, struct Combi &_Combination ) {/*{{{*/
    _JSONobject.at( "name").get_to( _Combination.name );
    _JSONobject.at( "instruments" ).get_to( _Combination.instruments );
}/*}}}*/
