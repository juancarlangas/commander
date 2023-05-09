#ifndef DATABASES_HPP
#define DATABASES_HPP

#include "../common/common.hpp"
#include "../data/combinations.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <array>
#include <vector>

#include "nlohmann/json.hpp"

#define ROWS 1000
#define MAXIMO_DE_CANCIONES 1000
#define N_FAVORITOS 10
static const std::int16_t &TRACKS_PER_PERFORMANCE{ 8 };

enum Switch : bool { OFF = 0, ON };

// NEW STRUCT TYPE{{{
struct Metadata {
	std::string title;
	std::string artist;
	std::string genre;
	std::string mood;
	std::string keyword;
};

struct Patch {
	std::int32_t bnk;
	std::int32_t num;
};

struct Settings {
	enum Switch state;
	std::int16_t volume;
	std::int16_t lower_key;
	std::int16_t upper_key;
	std::int16_t transposition;
};

struct Scene {
	std::string label;
	std::array<Settings, TRACKS_PER_PERFORMANCE> tracks;
};

struct Performance {
	Metadata metadata;
	Patch patch;
	std::string type;
	std::array<std::string, 8> instruments;
	std::vector<Scene> scenes;
	std::int16_t initial_scene;
};/*}}}*/

// OLD STRUCT TYPE{{{
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
};/*}}}*/

class Database {/*{{{*/
	public:
		Database();
		Database( const std::string & ) noexcept;
		Database( const std::string &, Combinations *_CombinationPtr ) noexcept;
		void load_from_json( const std::string &_Path);
		void from_new_to_old() noexcept;
		void load_csv( const std::string &_Path ) noexcept;
		void cargar( const std::string &_Path ) noexcept
		{
			load_from_json( _Path );
		}

		int32_t get_activeRows() noexcept;
		void cargar_especifico( const std::string &, int32_t ) noexcept;
		void clonar_to_old( Database & ) noexcept;
		void add_value(System);
		void edit_value(int, System);
		void delete_value(int);
		void ordenate();
		void delete_duplicated() noexcept;
		void clonar_from_old( Database & ) noexcept;
		void write_csv( const std::string &_Path ) noexcept;
		void write_json( const std::string &_Path ) noexcept;
		void escribir( const std::string &_Path ) noexcept
		{
			write_csv( _Path );
		}
		struct System get_cancion( const int ) noexcept;
		struct System *get_cancion_ptr( const int32_t &_Index ) noexcept;
		struct System *get_favourite_row( const int32_t & ) noexcept;
		struct System base[1000];
	private:
		std::vector<Performance> performances;
		std::int32_t activeRows;
		std::int32_t n_canciones;
		void clean_row(int);
		const char *homedir;
		struct System *favorito[ N_FAVORITOS ];
		void fill_favourites() noexcept;
		Combinations *combinationsPtr;

		friend class Playlist;
	};/*}}}*/

// JSON{{{
void from_json( const nlohmann::json &_JSONobject, Patch &_Patch );
void from_json( const nlohmann::json &_JSONobject, Settings &_Settings );
void from_json( const nlohmann::json &_JSONobject, Performance &_Performance );
void from_json( const nlohmann::json &_JSONobject, Scene &_Scene );
void from_json( const nlohmann::json &_JSONobject, Metadata &_Metadata );/*}}}*/

#endif
