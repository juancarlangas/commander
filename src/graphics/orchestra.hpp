#ifndef ORCHESTRA_HPP
#define ORCHESTRA_HPP

#include "graphics/window.hpp"
#include "graphics/fields.hpp"
#include "data/databases.hpp"
#include "elements/text_popup.hpp"

class KeyboardScheme : public TextPopup {
	public:
		void init(	const int32_t _Ysize, const int32_t _Xsize,
					const int32_t _Ypos, const int32_t _Xpos ) noexcept;
};

class Orchestra {
	public:
		void init( const int32_t, const int32_t, const int32_t, const int32_t ) noexcept;
		void show( struct System *& ) noexcept;
		void hide() noexcept;
		void capture_key() noexcept;
	private:
		Popup base;
		TextPopup variacion_text_box;
		TextPopup keyboard_scheme;
		Field etiqueta_field;
		int16_t variacion;
		struct System *info;
};

#endif
