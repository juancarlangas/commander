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

#include "data/Catalog.hpp"
#include "common/string.hpp"
#include "midi/midi.hpp"
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
	performances[line].tagging.title.clear();
	performances[line].tagging.genre.clear();
	performances[line].tagging.mood.clear();
	performances[line].tagging.keyword.clear();
	performances[line].type.clear();
	performances[line].program.bnk = 0;
	performances[line].program.num = 0;
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
		if (performances[i].tagging.keyword.starts_with("Favourite"))
			favourites[
				std::stoi(
					performances[i].tagging.keyword.substr(
						performances[i].tagging.keyword.find_first_of('_') + 1, std::string::npos)) - 1] = &performances[i];
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
			if (performances[a].tagging.mood != "Sound") { // If actual is not "Sound"
				b = a + 1; // Start searching from next
				found_keyword = false;
				while (found_keyword == false && b < static_cast<std::size_t>(activeRows)) {
					if (performances[b].tagging.mood != "Sound")
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
			if (performances[a].tagging.mood != "Lobby") {
				b = a + 1;
				found_keyword = false;
				while (found_keyword == false && b < static_cast<std::size_t>(activeRows)) {
					if (performances[b].tagging.mood != "Lobby")
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
			if (performances[a].tagging.mood != "Cena") {
				b = a + 1;
				found_keyword = false;
				while (found_keyword == false && b < static_cast<std::size_t>(activeRows)) {
					if (performances[b].tagging.mood != "Cena")
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
			if (performances[a].tagging.mood != "Baile") {
				b = a + 1;
				found_keyword = false;
				while (found_keyword == false && b < static_cast<std::size_t>(activeRows)) {
					if (performances[b].tagging.mood != "Baile")
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
				if (performances[a].tagging.mood == performances[b].tagging.mood) {
					k = 0;
					found_keyword = false;
					while (found_keyword == false && k < LONG_STRING) {
						if (performances[a].tagging.title[k] > performances[b].tagging.title[k]) {
							aux_performance = performances[a];
							performances[a] = performances[b];
							performances[b] = aux_performance;
							found_keyword = true;
						} else if (performances[a].tagging.title[k] < performances[b].tagging.title[k])
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
			if ((performances[i].tagging.title == performances[j].tagging.title ) and
				(performances[i].tagging.artist == performances[j].tagging.artist ) ) {
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
	return favourites[_FavNumber];
}/*}}}*/

auto Catalog::set_sfz_folder(const std::filesystem::path& _Folder) noexcept -> void {/*{{{*/
	sfz_folder = _Folder;
}/*}}}*/

auto Catalog::get_sfz_folder() const noexcept -> std::filesystem::path {/*{{{*/
	return sfz_folder;
}/*}}}*/

/************************ from_json *************************************/
// Overload for Tagging struct{{{
void from_json(const nlohmann::json& j, Tagging& m) {
	m.title = j.at("title").get<std::string>();
	m.artist = j.at("artist").get<std::string>();
	m.genre = j.at("genre").get<std::string>();
	m.mood = j.at("mood").get<std::string>();
	m.keyword = j.at("keyword").get<std::string>();
}/*}}}*/

// Overload for Program struct{{{
void from_json(const nlohmann::json& j, Program& p) {
	p.bnk = j.at("bnk").get<std::int32_t>();
	p.num = j.at("num").get<std::int32_t>();
}/*}}}*/

// Overload for Swith enum{{{
void from_json(const nlohmann::json& j, Switch& s) {
	static const std::unordered_map<std::string, Switch> state_map = {
		{"Off", Switch::OFF},
		{"On", Switch::ON}
	};
	auto it = state_map.find(j.get<std::string>());
	if (it == state_map.end()) {
		throw std::invalid_argument("Invalid State value");
	}
	s = it->second;
}/*}}}*/

// Overload for Strip struct{{{
void from_json(const nlohmann::json& j, Strip& s) {
	s.state = j.at("state").get<Switch>();
	s.midi_ch = j.at("midi_port").get<std::int16_t>();
	s.midi_ch = j.at("midi_ch").get<std::int16_t>();
	s.volume = j.at("volume").get<std::int16_t>();
	s.lower_key = j.at("lo_key").get<std::int16_t>();
	s.upper_key = j.at("hi_key").get<std::int16_t>();
	s.transposition = j.at("transposition").get<std::int16_t>();
}/*}}}*/

// Overload for Scene struct{{{
void from_json(const nlohmann::json& j, Scene& sc) {
	sc.label = j.at("label").get<std::string>();
	auto& strips = j.at("strips");
	for (std::size_t i = 0; i < strips.size(); ++i) {
		from_json(strips[i], sc.strips[i]);
	}
}/*}}}*/

// Overload for Performance struct{{{
void from_json(const nlohmann::json& j, Performance& p) {
	from_json(j.at("tagging"), p.tagging);
	from_json(j.at("program"), p.program);
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

	p.tagging = p.tagging;
	p.program = p.program;
}/*}}}*/

/*********************** to_json ***************************************/
// Overload for Tagging struct{{{
void to_json(nlohmann::ordered_json& j, const Tagging& m) {
	j = nlohmann::ordered_json{{"title", m.title},
							   {"artist", m.artist},
							   {"genre", m.genre},
							   {"mood", m.mood},
							   {"keyword", m.keyword}};
}/*}}}*/

// Overload for Program struct{{{
void to_json(nlohmann::ordered_json& j, const Program& p) {
	j = nlohmann::ordered_json{{"bnk", p.bnk},
							   {"num", p.num}};
}/*}}}*/

// Overload for Switch enum{{{
void to_json(nlohmann::ordered_json& j, const Switch& s) {
	static const std::unordered_map<Switch, std::string> state_map = {
		{Switch::OFF, "Off"},
		{Switch::ON, "On"}
	};
	auto it = state_map.find(s);
	if (it == state_map.end()) {
		throw std::invalid_argument("Invalid State value");
	}
	j = it->second;
}/*}}}*/

// Overload for Strip struct{{{
void to_json(nlohmann::ordered_json& j, const Strip& s) {
	j = nlohmann::ordered_json{{"state", s.state},
							   {"midi_port", s.midi_port},
							   {"midi_ch", s.midi_ch},
							   {"volume", s.volume},
							   {"lo_key", s.lower_key},
							   {"hi_key", s.upper_key},
							   {"transposition", s.transposition}};
}/*}}}*/

// Overload for Scene struct{{{
void to_json(nlohmann::ordered_json& j, const Scene& sc) {
	j = nlohmann::ordered_json{{"label", sc.label},
							   {"strips", sc.strips}};
}/*}}}*/

// Overload for Performance struct{{{
void to_json(nlohmann::ordered_json& j, const Performance& p) {
	j = nlohmann::ordered_json{{"tagging", p.tagging},
							   {"program", p.program},
							   {"type", p.type},
							   {"n_scenes", p.n_scenes },
							   {"scenes", p.scenes},
							   {"default_scene", p.default_scene},
							   {"sfz_file", p.sfz_filename.empty() ? "empty.sfz" : p.sfz_filename}};
							   //{"sfz_file", ""}};
}/*}}}*/
