#ifndef DATABASES_HPP
#define DATABASES_HPP

#include "../midi/midi.hpp"

#include <cstdint>
#include <string>
#include <array>
#include <vector>

#include "nlohmann/json.hpp"

#define ROWS 1000
#define MAXIMO_DE_CANCIONES 1000
#define N_FAVORITOS 10
static const std::int16_t &TRACKS_PER_PERFORMANCE{ 8 };


// NEW STRUCT TYPE{{{
struct Metadata {
	std::string title;
	std::string artist;
	std::string genre;
	std::string mood;
	std::string keyword;
};

struct Patch {
	std::int32_t bnk{ 0 };
	std::int32_t num{ 0 };
};

struct Settings {
	enum Switch state{ Switch::OFF };
	std::int16_t volume{ 100 };
	std::int16_t lower_key{ 24 };
	std::int16_t upper_key{ 72 };
	std::int16_t transposition{ 0 };
};

struct Scene {
	std::string label;
	std::array<Settings, TRACKS_PER_PERFORMANCE> tracks;
};

struct Performance { Metadata metadata;
	Patch patch;
	std::string type;
	std::int16_t n_scenes{ 0 };
	std::vector<Scene> scenes;
	std::int16_t default_scene{ 0 };
};/*}}}*/

class Catalog {/*{{{*/
	public:
		Catalog();
		Catalog( const std::string & ) noexcept;
		void load_from_json( const std::string &_Path);
		int32_t get_activeRows() noexcept;
		void add_value(const Performance& _Performance);
		void edit_value(std::int32_t, const Performance& _Performance);
		void delete_value(int);
		void ordenate();
		void delete_duplicated() noexcept;
		void save_to_json( const std::string &_Path ) noexcept;
		Performance get_cancion( const int ) noexcept;
		Performance *get_cancion_ptr( const int32_t &_Index ) noexcept;
		Performance *get_favourite_row( const int32_t & ) noexcept;
		std::vector<Performance> performances;
	private:
		std::int32_t activeRows;
		std::int32_t n_canciones;
		void clean_row(int);
		const char *homedir;
		std::array<Performance*, N_FAVORITOS>favourites;
		auto fill_favourites() noexcept -> void;

		friend class Playlist;
	};/*}}}*/

// JSON{{{
void from_json( const nlohmann::json &_JSONobject, Patch &_Patch );
void from_json( const nlohmann::json &_JSONobject, Settings &_Settings );
void from_json( const nlohmann::json& j, Switch& s);
void from_json( const nlohmann::json &_JSONobject, Performance &_Performance );
void from_json( const nlohmann::json &_JSONobject, Scene &_Scene );
void from_json( const nlohmann::json &_JSONobject, Metadata &_Metadata );

void to_json(nlohmann::ordered_json& j, const Metadata& m);
void to_json(nlohmann::ordered_json& j, const Patch& p);
void to_json(nlohmann::ordered_json& j, const Switch& s);
void to_json(nlohmann::ordered_json& j, const Settings& s);
void to_json(nlohmann::ordered_json& j, const Scene& sc);
void to_json(nlohmann::ordered_json& j, const Performance& p);/*}}}*/


#endif
