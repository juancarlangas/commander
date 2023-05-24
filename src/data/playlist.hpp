#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP

#include "data/catalog.hpp"
#include "utilities/src/files.hpp"
#include <array>
#include <cstdint>
#include <string>
#include <string_view>

struct Pista {
	std::string titulo;
	std::string artista;
	Performance* row_ptr;
};

class Playlist {
	public:
		Playlist( Catalog *_Catalog_ptr ); // solo inicializa
		Playlist( const std::string &_Path, Catalog *_Catalog_ptr );

		void cargar( const std::string &_Path ) noexcept;
		void guardar( const std::string &_Path) noexcept;

		void agregar( Performance *&_Row_ptr ) noexcept;
		void eliminar( const int32_t &_Index ) noexcept;
		void sincronizar() noexcept;

		int32_t get_n_pistas() noexcept;

		const std::string &get_titulo( const int32_t &_Index ) noexcept;
		const std::string &get_artista( const int32_t &_Index ) noexcept;

		/* Esta función es la efectiva para mandar la información al teclado.
		 * Mandará el APUNTADOR correcto hacia el renglón en la base de datos */
		Performance* get_pointer( const int32_t &_Index) noexcept;

		~Playlist();

	private:
		std::array<struct Pista, MAXIMO_DE_CANCIONES> pista;
		int32_t n_pistas;

		Catalog *database_ptr;
};

#endif
