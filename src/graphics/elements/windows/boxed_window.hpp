#ifndef BOXED_WINDOW_HPP
#define BOXED_WINDOW_HPP

#include "graphics/elements/window.hpp"

class BoxedWindow : public Window {
	public:
		void init( const int32_t, const int32_t, const int32_t, const int32_t,
					const int32_t ) noexcept;
		void set_box_color( const int32_t &color ) noexcept;
		void set_box_width( const std::string_view ) noexcept;
		void define_box(	const int32_t &_Top, const int32_t &_Right,
							const int32_t &_Bottom, const int32_t &_Left,
							const int32_t &_TopLeft, const int32_t &_TopRight,
							const int32_t &_BottomRight, const int32_t &_BottomLeft ) noexcept;
		void draw() noexcept;
		void update() noexcept;
	protected:
		WINDOW *box;
};

#endif
