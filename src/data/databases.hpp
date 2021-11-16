#ifndef DATABASES_HPP
#define DATABASES_HPP

#include "../common/common.hpp"

#include <string>
#include <string_view>
#include <array>

#define ROWS 1000
#define MAXIMO_DE_CANCIONES 1000

enum Switch : bool { OFF = 0, ON };

struct Track {
	enum Switch status;
	int16_t lower_key;
	int16_t upper_key;
	int16_t transposition;
};

struct Variacion {
	std::string etiqueta;
	std::array<struct Track, 8> track;
};

struct Cancion {
	std::string titulo;
	std::string artista;
	std::string genero;
	std::string mood;
	std::string keywords;
	std::string tipo;
	char bnk;
	int16_t num;
	int16_t n_variaciones;
	std::array<Variacion, 16> variacion;
	int16_t variacion_inicial;
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
		struct Cancion get_cancion( const int ) noexcept;

		System base[ROWS];
	private:
		int32_t activeRows;
		int32_t n_canciones;
		std::array<struct Cancion, MAXIMO_DE_CANCIONES> cancion;
		void clean_row(int);
		const char *homedir;
	};

#endif
