#include <bits/stdint-intn.h>
#include <cstddef>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>
#include <string_view>
#include <iostream>
#include <fstream>
#include <iomanip> // zerofill

#include "data/catalog.hpp"
#include "common/string.hpp"
#include "nlohmann/json.hpp"
#include "common/common.hpp"

Catalog::Catalog() :/*{{{*/
		activeRows( 0 ),
		n_canciones( 0 )
{
	// homedir
	if ((homedir = getenv("HOME")) == NULL)
		homedir = getpwuid(getuid())->pw_dir;
}/*}}}*/

Catalog::Catalog( const std::string &_Path ) noexcept/*{{{*/
{
	// homedir
	if ((homedir = getenv("HOME")) == NULL)
		homedir = getpwuid(getuid())->pw_dir;

	load_from_json( _Path );
}/*}}}*/

void Catalog::clean_row(int line)/*{{{*/
{
	performances[line].metadata.title.clear();
	performances[line].metadata.genre.clear();
	performances[line].metadata.mood.clear();
	performances[line].metadata.keyword.clear();
	performances[line].type.clear();
	performances[line].patch.bnk = 0;
	performances[line].patch.num = 0;
}/*}}}*/

void Catalog::load_from_json( const std::string &_Path)/*{{{*/
{
	// LOAD DATA
	std::ifstream json_file{ _Path };
	if ( json_file.fail() ) {
		std::cerr << "Failed to open " + _Path + " in Catalog::load_from_json()\n";
		exit(EXIT_FAILURE);
	}
	nlohmann::json json_object;
	json_file >> json_object;
	json_object.get_to(performances);

	json_file.close();

	activeRows = n_canciones = performances.size();

	ordenate();
}/*}}}*/

auto Catalog::fill_favourites() noexcept -> void {/*{{{*/
	for (std::size_t i {0}; i < performances.size(); ++i)
		if (performances[i].metadata.keyword.starts_with("Favourite"))
			favourites[
				std::stoi(
					performances[i].metadata.keyword.substr(
						performances[i].metadata.keyword.find_first_of('_') + 1, std::string::npos)) - 1] = &performances[i];
}/*}}}*/

int32_t Catalog::get_activeRows() noexcept/*{{{*/
{
	return n_canciones;
}/*}}}*/

void Catalog::save_to_json(const std::string& _Path) noexcept {/*{{{*/
    // Create a JSON object and fill it with the data from performances vector
    nlohmann::ordered_json json_object = performances;

    // Open a file and write the JSON object to it
    std::ofstream json_file{ _Path };
    if (json_file.fail()) {
        std::cerr << "Failed to open " + _Path + " in Catalog::save_to_json()\n";
        exit(EXIT_FAILURE);
    }
    json_file << std::setfill('\t') << std::setw(1) << json_object << std::endl;
    json_file.close();
}/*}}}*/

void Catalog::add_value( const Performance& _Performance )/*{{{*/
{
	performances.push_back(_Performance);

	n_canciones = ++activeRows;

	ordenate();
}/*}}}*/

void Catalog::edit_value(const std::int32_t line, const Performance& _Performance)/*{{{*/
{
	performances[ line ] = _Performance;

	ordenate();
}/*}}}*/

void Catalog::delete_value( int line )/*{{{*/
{
	performances.erase(std::begin(performances) + line);
	n_canciones = --activeRows;

	ordenate();
}/*}}}*/

void Catalog::ordenate()/*{{{*/
{
	std::size_t a, b;
	int32_t k;
	bool found_keyword;
	Performance aux_performance;

	a = 0;
	if (activeRows > 1) {
		// Mood: Sound
		a = 0;
		while (a < static_cast<std::size_t>(activeRows - 1) and b < static_cast<std::size_t>(activeRows - 1)) {	
			if (performances[a].metadata.mood != "Sound") { // If actual is not "Sound"
				b = a + 1; // Start searching from next
				found_keyword = false;
				while (found_keyword == false && b < static_cast<std::size_t>(activeRows)) {
					if (performances[b].metadata.mood != "Sound")
						++b; //continue searching
					else {
						aux_performance = performances[a];
						performances[a] = performances[b];
						performances[b] = aux_performance;
						found_keyword = true;
					}
				}
			}
			++a;
		}

		// mood: Lobby
		a--;
		b = a + 1;
		while (a < static_cast<std::size_t>(activeRows - 1) and b < static_cast<std::size_t>(activeRows - 1)) {	
			if (performances[a].metadata.mood != "Lobby") {
				b = a + 1;
				found_keyword = false;
				while (found_keyword == false && b < static_cast<std::size_t>(activeRows)) {
					if (performances[b].metadata.mood != "Lobby")
						b++;
					else {
						aux_performance = performances[a];
						performances[a] = performances[b];
						performances[b] = aux_performance;
						found_keyword = true;
					}
				}
			}
			a++;
		}

		// mood: "Cena"
		a--;
		b = a + 1;
		while (a < static_cast<std::size_t>(activeRows - 1) and b < static_cast<std::size_t>(activeRows - 1)) {	
			if (performances[a].metadata.mood != "Cena") {
				b = a + 1;
				found_keyword = false;
				while (found_keyword == false && b < static_cast<std::size_t>(activeRows)) {
					if (performances[b].metadata.mood != "Cena")
						b++;
					else {
						aux_performance = performances[a];
						performances[a] = performances[b];
						performances[b] = aux_performance;
						found_keyword = true;
					}
				}
			}
			a++;
		}

		//Baile
		a--;
		b = a + 1;
		while (a < static_cast<std::size_t>(activeRows - 1) and b < static_cast<std::size_t>(activeRows - 1)) {	
			if (performances[a].metadata.mood != "Baile") {
				b = a + 1;
				found_keyword = false;
				while (found_keyword == false && b < static_cast<std::size_t>(activeRows)) {
					if (performances[b].metadata.mood != "Baile")
						b++;
					else {
						aux_performance = performances[a];
						performances[a] = performances[b];
						performances[b] = aux_performance;
						found_keyword = true;
					}
				}
			}
			a++;
		}

		// ALphabet
		for (a = 0; a < static_cast<std::size_t>(activeRows - 1); a++)
			for (b = a + 1; b < static_cast<std::size_t>(activeRows); b++) {
				if (performances[a].metadata.mood == performances[b].metadata.mood) {
					k = 0;
					found_keyword = false;
					while (found_keyword == false && k < LONG_STRING) {
						if (performances[a].metadata.title[k] > performances[b].metadata.title[k]) {
							aux_performance = performances[a];
							performances[a] = performances[b];
							performances[b] = aux_performance;
							found_keyword = true;
						} else if (performances[a].metadata.title[k] < performances[b].metadata.title[k])
							found_keyword = true;
						k++;
					}
				}
			}
	}

	fill_favourites();
}/*}}}*/

void Catalog::delete_duplicated() noexcept/*{{{*/
{
	int32_t i, j;
	for ( i = 0; i < n_canciones - 1; ++i )
		for ( j = i + 1; j < n_canciones; ++j )
			if ((performances[i].metadata.title == performances[j].metadata.title ) and
				(performances[i].metadata.artist == performances[j].metadata.artist ) ) {
				delete_value( j );
				--n_canciones;
			}
	ordenate();
}/*}}}*/

Performance Catalog::get_cancion( const int _Index ) noexcept/*{{{*/
{
	return performances[ _Index ];
}/*}}}*/

Performance *Catalog::get_cancion_ptr( const int32_t &_Index ) noexcept/*{{{*/
{
	return &performances[_Index];
}/*}}}*/

Performance *Catalog::get_favourite_row( const int32_t &_FavNumber ) noexcept/*{{{*/
{
	return favourites[ _FavNumber ];
}/*}}}*/

auto Catalog::set_sfz_folder(const std::filesystem::path& _Folder) noexcept -> void {/*{{{*/
	sfz_folder = _Folder;
}/*}}}*/

auto Catalog::get_sfz_folder() const noexcept -> std::filesystem::path {/*{{{*/
	return sfz_folder;
}/*}}}*/

/************************************* from_json **************************************************/
// Overload for Metadata struct{{{
void from_json(const nlohmann::json& j, Metadata& m) {
	m.title = j.at("title").get<std::string>();
	m.artist = j.at("artist").get<std::string>();
	m.genre = j.at("genre").get<std::string>();
	m.mood = j.at("mood").get<std::string>();
	m.keyword = j.at("keyword").get<std::string>();
}/*}}}*/

// Overload for Patch struct{{{
void from_json(const nlohmann::json& j, Patch& p) {
	p.bnk = j.at("bnk").get<std::int32_t>();
	p.num = j.at("num").get<std::int32_t>();
}/*}}}*/

// Overload for Swith enum{{{
void from_json(const nlohmann::json& j, Switch& s) {
	static const std::unordered_map<std::string, Switch> switch_map = {
		{"OFF", OFF},
		{"ON", ON}
	};
	auto it = switch_map.find(j.get<std::string>());
	if (it == switch_map.end()) {
		throw std::invalid_argument("Invalid Switch value");
	}
	s = it->second;
}/*}}}*/

// Overload for Settings struct{{{
void from_json(const nlohmann::json& j, Settings& s) {
	s.state = j.at("state").get<Switch>();
	s.volume = j.at("volume").get<std::int16_t>();
	s.lower_key = j.at("lower_key").get<std::int16_t>();
	s.upper_key = j.at("upper_key").get<std::int16_t>();
	s.transposition = j.at("transposition").get<std::int16_t>();
}/*}}}*/

// Overload for Scene struct{{{
void from_json(const nlohmann::json& j, Scene& sc) {
	sc.label = j.at("label").get<std::string>();
	auto& tracks = j.at("tracks");
	for (std::size_t i = 0; i < tracks.size(); ++i) {
		from_json(tracks[i], sc.tracks[i]);
	}
}/*}}}*/

// Overload for Performance struct{{{
void from_json(const nlohmann::json& j, Performance& p) {
	from_json(j.at("metadata"), p.metadata);
	from_json(j.at("patch"), p.patch);
	p.type = j.at("type").get<std::string>();
	p.n_scenes = j.at("n_scenes").get<std::int16_t>();
	auto& scenes = j.at("scenes");
	for (std::size_t i = 0; i < scenes.size(); ++i) {
		Scene sc;
		from_json(scenes[i], sc);
		p.scenes.push_back(sc);
	}
	p.default_scene = j.at("default_scene").get<std::int16_t>();
	p.sfz_filename = j.at("sfz_file").get<std::string>();

	p.tagging = p.metadata;
	p.program = p.patch;
}/*}}}*/

/*************************************** to_json ****************************************************/
// Overload for Metadata struct{{{
void to_json(nlohmann::ordered_json& j, const Metadata& m) {
	j = nlohmann::ordered_json{{"title", m.title},
							   {"artist", m.artist},
							   {"genre", m.genre},
							   {"mood", m.mood},
							   {"keyword", m.keyword}};
}/*}}}*/

// Overload for Patch struct{{{
void to_json(nlohmann::ordered_json& j, const Patch& p) {
	j = nlohmann::ordered_json{{"bnk", p.bnk},
							   {"num", p.num}};
}/*}}}*/

// Overload for Switch enum{{{
void to_json(nlohmann::ordered_json& j, const Switch& s) {
	static const std::unordered_map<Switch, std::string> switch_map = {
		{OFF, "OFF"},
		{ON, "ON"}
	};
	auto it = switch_map.find(s);
	if (it == switch_map.end()) {
		throw std::invalid_argument("Invalid Switch value");
	}
	j = it->second;
}/*}}}*/

// Overload for Settings struct{{{
void to_json(nlohmann::ordered_json& j, const Settings& s) {
	j = nlohmann::ordered_json{{"state", s.state},
							   {"volume", s.volume},
							   {"lower_key", s.lower_key},
							   {"upper_key", s.upper_key},
							   {"transposition", s.transposition}};
}/*}}}*/

// Overload for Scene struct{{{
void to_json(nlohmann::ordered_json& j, const Scene& sc) {
	j = nlohmann::ordered_json{{"label", sc.label},
							   {"tracks", sc.tracks}};
}/*}}}*/

// Overload for Performance struct{{{
void to_json(nlohmann::ordered_json& j, const Performance& p) {
	j = nlohmann::ordered_json{{"metadata", p.metadata},
							   {"patch", p.patch},
							   {"type", p.type},
							   {"n_scenes", p.n_scenes },
							   {"scenes", p.scenes},
							   {"default_scene", p.default_scene},
							   {"sfz_file", p.sfz_filename.empty() ? "empty.sfz" : p.sfz_filename}};
							   //{"sfz_file", ""}};
}/*}}}*/
