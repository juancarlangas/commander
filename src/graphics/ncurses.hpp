#ifndef NCURSES_HPP
#define NCURSES_HPP

#define Y_PIXELS 32
#define X_PIXELS 127

#include <cstdint>

void ncurses_start_sequence() noexcept;
void ncurses_end_sequence() noexcept;
auto set_windows(void) noexcept -> void;
void draw_windows(void);
void tint_lcd(const short int mode);
void update_popups() noexcept;
auto get_y_pixels() noexcept -> std::int16_t;

class Screen {
	public:
		Screen();
		~Screen();
		void on() noexcept;
	private:
		friend class ventana;
};

#endif
