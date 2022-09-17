#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP

#include "data/databases.hpp"
#include "utilities/src/files.hpp"
#include <array>
#include <cstdint>
#include <string>
#include <string_view>

struct Pista {
	std::string titulo;
	std::string artista;
	struct System *row_ptr;
};

class Playlist {
	public:
		Playlist( const std::string &_Path, Database *_Database_ptr );

		void cargar( const std::string &_Path ) noexcept;
		void guardar( const std::string &_Path) noexcept;

		void agregar(	const std::string &_Titulo, const std::string &_Artista,
						struct System *&_Row_ptr ) noexcept;
		void eliminar( const int32_t &_Index ) noexcept;
		void sincronizar() noexcept;

		int32_t get_n_pistas() noexcept;
		const std::string &get_titulo( const int32_t &_Index ) noexcept;
		const std::string &get_artista( const int32_t &_Index ) noexcept;

		~Playlist();

	private:
		std::array<struct Pista, MAXIMO_DE_CANCIONES> pista;
		int32_t n_pistas;

		Database *database_ptr;
};

#endif
