#ifndef DOUBLE_X_SLIDER_HPP
#define DOUBLE_X_SLIDER_HPP

#include "ui/elements/windows/popups/orchestra_element.hpp"
#include <bits/stdint-intn.h>

#define MIN_KEY 36
#define MAX_KEY 96

class DoubleXslider : public OrchestraElement {
	public:
		DoubleXslider();
		void init( const int32_t &, const int32_t &, const int32_t &, const int32_t & ) noexcept;
		void set_limits( const int16_t &_Lvalue, const int16_t &_Rvalue ) noexcept;
		void on() noexcept;
		void off() noexcept;
		auto light() noexcept -> void;
		void set_values( const int16_t &, const int16_t & ) noexcept;
		void leave_cursor() noexcept;
		void set_cursor_at_left() noexcept;
		void set_cursor_at_right() noexcept;
		void swap_cursor() noexcept;
		bool decrease_left_slider() noexcept;
		bool decrease_right_slider() noexcept;
		bool increase_left_slider() noexcept;
		bool increase_right_slider() noexcept;
		void clean() noexcept;
	private:
		bool is_active;
		int16_t focused_slider;
		int16_t min_value, max_value;
		int16_t lValue, rValue;
		int16_t lPos, rPos;
};

#endif
