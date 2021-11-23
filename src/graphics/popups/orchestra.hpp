#ifndef ORCHESTRA_HPP
#define ORCHESTRA_HPP

#include "graphics/popup.hpp"
#include "graphics/fields.hpp"

class Orchestra : public Popup {
	public:
		void init( const int32_t _Ysize, const int32_t _Xsize,
					const int32_t _Xpos, const int32_t _Ypos ) noexcept;
	private:
		Field etiqueta_field;
};

#endif
