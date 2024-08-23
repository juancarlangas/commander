#ifndef ORCHESTRA_HPP
#define ORCHESTRA_HPP

#include "ui/elements/window.hpp"
#include "ui/fields.hpp"
#include "midi/Keyboard.hpp"
#include "data/Catalog.hpp"
#include "ui/elements/windows/boxed_windows/boxed_popup.hpp"
#include "elements/windows/popups/orchestra_elements/check_popup.hpp"
#include "elements/windows/popups/orchestra_elements/text_popups/field_popup.hpp"
#include "elements/windows/popups/orchestra_elements/double_X_slider.hpp"
#include "midi/Keyboard.hpp"
#include <jack/types.h>

enum Notas {C, C_SUS, D, D_SUS, E, F, F_SUS, G, G_SUS, A, A_SUS, B};
enum Coordinates {X, Y};
enum CursorX {
	DEFAULT_SCENE = 0,
	SCENE_LABEL,
	STATE = 0,
	MIDI_CH,
	INSTRUMENT,
	VOLUME,
	TRANSPOSITION,
	L_SLIDER,
	R_SLIDER
};
enum Moved : bool {NO, YES};

struct Clipboard {
	int16_t n_variaciones;
	Scene variacion[ MAX_VARIATIONS ];
};

class KeyboardScheme : public TextPopup {
	public:
		void auto_draw() noexcept;
};

class Orchestra {
	public:
		Orchestra();
		void init( const int32_t, const int32_t, const int32_t, const int32_t ) noexcept;
		void show( Performance *&_Performance ) noexcept;
		void update() noexcept;
		void hide() noexcept;
		void capture_key() noexcept;
		void link_MIDI_device( Keyboard * ) noexcept;
		void reset_variation() noexcept;
		auto add_empty_scene(Performance*& _Performance) noexcept -> void;
	private:
		std::string temp_word;
		BoxedPopup base;
			TextPopup scene_text_box;
			KeyboardScheme keyboard_scheme;
			Field vi_field;
			Field etiqueta_field;
				std::array<CheckPopup, NUMBER_OF_PARTS> status_field;
				std::array<FieldPopup, NUMBER_OF_PARTS> port;
				std::array<FieldPopup, NUMBER_OF_PARTS> channel_field;
				std::array<FieldPopup, NUMBER_OF_PARTS> instrument_field;
				std::array<FieldPopup, NUMBER_OF_PARTS> volume_field;
				std::array<FieldPopup, NUMBER_OF_PARTS> transposition_field;
				std::array<DoubleXslider, NUMBER_OF_PARTS> double_X_slider;
				std::array<int16_t, 2> cursor;
		std::array<struct Font, NUMBER_OF_PARTS> native_font;
		struct Font cursor_font, MIDI_font, dimmed_font, light_font;
		int16_t current_scene;
		Performance *info;
		Keyboard *keyboard_ptr;
		struct Clipboard clipboard;
};

#endif
