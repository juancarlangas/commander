#ifndef COMBINATIONS_HPP
#define COMBINATIONS_HPP

#include <array>
#include <bits/stdint-intn.h>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

#include "data/nlohmann/json.hpp"

static const size_t &PATCHES_PER_BANK{ 128 };
static const size_t& INSTRUMENTS_PER_COMBI{ 8 };

struct Combi {
	std::string name;
	std::array<std::string, INSTRUMENTS_PER_COMBI> instruments;
};

struct Row {
	char bnk;
	int16_t num;
	std::string nombre;
	std::array<std::string, 8> instrumento;
};

class Combinations {
	public:
		Combinations( const std::string & );
		~Combinations();
		void load_from_json( const std::string & );
		std::string get_instrument_name(
				const char &_Banco, const int16_t &_Num, const int16_t &_Track ) noexcept;
		void set_instrument_name( const char &, const int16_t &, const int16_t &,
				const std::string_view ) noexcept;
		size_t channels_per_combi;
	private:
		std::vector<std::array<struct Combi, PATCHES_PER_BANK>> combinations;
		size_t n_bancos;
		int16_t bnk_num_to_int( const char &, const int16_t & ) noexcept;
};
		
void from_json( const nlohmann::json &_JSONobject, struct Combi &_Combination );

#endif
