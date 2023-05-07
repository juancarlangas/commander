#ifndef COMBINATIONS_HPP
#define COMBINATIONS_HPP

#include <array>
#include <bits/stdint-intn.h>
#include <string>
#include <string_view>
#include <vector>

struct Combi {
	std::string name;
	std::array<std::string, 8> instrument_list;
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
		void load_from_csv( const std::string & ) noexcept;
		void load_from_json( const std::string & ) noexcept;
		void escribir( const std::string & ) noexcept;
		std::string get_instrument_name(
				const char &_Banco, const int16_t &_Num, const int16_t &_Track ) noexcept;
		void set_instrument_name( const char &, const int16_t &, const int16_t &,
				const std::string_view )
			noexcept;
	private:
		int16_t n_bancos;
		struct Row *data;
		std::vector<std::array<struct Combi, 128>> combination_list;
		int16_t bnk_num_to_int( const char &, const int16_t & ) noexcept;
};

#endif
