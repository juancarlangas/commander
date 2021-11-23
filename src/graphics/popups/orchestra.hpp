#ifndef ORCHESTRA_HPP
#define ORCHESTRA_HPP

#include "graphics/popup.hpp"
#include "graphics/fields.hpp"

class Orchestra : public Popup {
	public:
		void init( const int32_t _Ysize, const int32_t _Xsize,
					const int32_t _Xpos, const int32_t _Ypos ) noexcept;
		void show() noexcept;
		void hide() noexcept;
		void capture_key() noexcept;
	private:
		Popup base;
		Field etiqueta_field;
};

#endif
