#ifndef DATABASES_HPP
#define DATABASES_HPP

#include "../common/common.hpp"

#include <string>
#include <string_view>
#include <array>

#define ROWS 1000
#define MAXIMO_DE_PISTAS 1000

enum Switch : bool { OFF = 0, ON };

struct Arreglo {
	std::string etiqueta;
	std::array<bool, 16> canal;
};

struct Pista {
	std::string titulo;
	std::string artista;
	std::string genero;
	std::string mood;
	std::string key_words;
	std::string tipo;
	char bnk;
	int16_t num;
	int16_t n_arreglos;
	std::array<Arreglo, 8> orquestacion;
	int16_t arreglo_inicial;
};

struct System {
		char title[LONG_STRING];
		char artist[LONG_STRING];
		char genre[LONG_STRING];
		char section[LONG_STRING];
		char keywords[LONG_STRING];
		char type[10];
		char bnk;
		short num;
	};

class Database {
	public:
		Database();
		void load(const char*, const char *);
		int get_activeRows(const char *,const char *);
		void cargar( const std::string & ) noexcept;
		void clonar_to_old( Database & ) noexcept;
		void add_value(System);
		void edit_value(int, System);
		void delete_value(int);
		void ordenate();
		void exportate(const char *);
		void clonar_from_old( Database & ) noexcept;
		void escribir( const std::string & ) noexcept;

		System base[ROWS];
	private:
		int32_t activeRows;
		std::array<Pista, MAXIMO_DE_PISTAS> catalogo;
		void clean_row(int);
		const char *homedir;
	};

#endif
