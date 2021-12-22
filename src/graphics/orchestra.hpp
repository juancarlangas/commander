#ifndef ORCHESTRA_HPP
#define ORCHESTRA_HPP

#include "graphics/ncurses.hpp"
#include "graphics/elements/window.hpp"
#include "graphics/fields.hpp"
#include "data/combinations.hpp"
#include "data/databases.hpp"
#include "graphics/elements/windows/boxed_windows/boxed_popup.hpp"
#include "elements/windows/popups/orchestra_elements/check_popup.hpp"
#include "elements/windows/popups/orchestra_elements/text_popups/field_popup.hpp"
#include "elements/windows/popups/orchestra_elements/double_X_slider.hpp"
#include "midi/keyboards.hpp"

enum Notas { C, C_SUS, D, D_SUS, E, F, F_SUS, G, G_SUS, A, A_SUS, B };
enum Coordinates { X, Y };
enum Moved : bool { NO, YES };

struct Clipboard {
	int16_t n_variaciones;
	struct Variacion variacion[ MAX_VARIATIONS ];
};

class KeyboardScheme : public TextPopup {
	public:
		void auto_draw() noexcept;
};

class Orchestra {
	public:
		Orchestra();
		void init( const int32_t, const int32_t, const int32_t, const int32_t ) noexcept;
		void show( struct System *& ) noexcept;
		void update() noexcept;
		void hide() noexcept;
		void capture_key() noexcept;
		void link_combinations( Combinations * ) noexcept;
		void link_MIDI_device( Keyboard * ) noexcept;
		void reset_variation() noexcept;
	private:
		std::string temp_word;
		BoxedPopup base;
			TextPopup variacion_text_box;
			KeyboardScheme keyboard_scheme;
			Field etiqueta_field;
				std::array<CheckPopup, 8> status_field;
				std::array<FieldPopup, 8> instrument_field;
				std::array<FieldPopup, 8> transposition_field;
				std::array<DoubleXslider, 8> double_X_slider;
				std::array<int16_t, 2> cursor;
		std::array<struct Font, 8> native_font;
		struct Font cursor_font, MIDI_font, dimmed_font;
		int16_t variacion;
		struct System *info;
		Combinations *comb_ptr;
		Keyboard *keyboard;
		struct Clipboard clipboard;
};

#endif
