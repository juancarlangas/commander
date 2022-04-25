#ifndef DATABASES_HPP
#define DATABASES_HPP

#include "../common/common.hpp"
#include "../data/combinations.hpp"

#include <string>
#include <string_view>
#include <array>

#define ROWS 1000
#define MAXIMO_DE_CANCIONES 1000
#define N_FAVORITOS 10

enum Switch : bool { OFF = 0, ON };

struct Track {
	enum Switch status;
	int16_t volume;
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
		Database( const std::string & ) noexcept;
		Database( const std::string &, Combinations *_CombinationPtr ) noexcept;
		~Database();
		void cargar( const std::string & ) noexcept;
		int32_t get_activeRows() noexcept;
		void cargar_especifico( const std::string &, int32_t ) noexcept;
		void clonar_to_old( Database & ) noexcept;
		void add_value(System);
		void edit_value(int, System);
		void delete_value(int);
		void ordenate();
		void exportate(const char *);
		void clonar_from_old( Database & ) noexcept;
		void escribir( const std::string & ) noexcept;
		struct System get_cancion( const int ) noexcept;
		struct System *get_favourite_row( const int32_t & ) noexcept;
		System *base;
	private:
		int32_t activeRows;
		int32_t n_canciones;
		void clean_row(int);
		const char *homedir;
		struct System *favorito[ N_FAVORITOS ];
		void fill_favourites() noexcept;
		Combinations *combinationsPtr;
	};

#endif
