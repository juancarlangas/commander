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
	struct Track track[ 8 ];
};

struct System {
	// C
	char title[LONG_STRING];
	char artist[LONG_STRING];
	char genre[LONG_STRING];
	char section[LONG_STRING];
	char keywords[LONG_STRING];
	char type[10];
	
	// C++
	std::string titulo;
	std::string artista;
	std::string genero;
	std::string mood;
	std::string key_words;
	std::string tipo;

	// Global
	char bnk;
	short num;

	// New
	int16_t n_variaciones;
	int16_t variacion_inicial;
	std::string instrumento[ 8 ];
	struct Variacion variacion[ 16 ];
};

class Database {
	public:
		Database();
		~Database();
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
		struct System get_cancion( const int ) noexcept;
		System *base;
	private:
		int32_t activeRows;
		int32_t n_canciones;
		void clean_row(int);
		const char *homedir;
	};

#endif
