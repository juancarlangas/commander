#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP

#include "data/databases.hpp"
#include "utilities/src/files.hpp"
#include <array>
#include <cstdint>
#include <string>

struct Pista {
	std::string titulo;
	std::string artista;
};

class Playlist {
	public:
		Playlist( const std::string &_Path );
		void cargar( const std::string &_Path ) noexcept;
		~Playlist();
	private:
		std::array<struct Pista, MAXIMO_DE_CANCIONES> pista;
		int32_t n_pistas;
};

#endif
