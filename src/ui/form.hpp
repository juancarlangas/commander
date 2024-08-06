#ifndef FORM_H
#define FORM_H

#include "ui/fields.hpp"
#include "data/Catalog.hpp"

enum Encuadre {TITLE, ARTIST, GENRE, SECTION, TYPE, KEYWORDS, BNK, NUM, SFZ};

enum Action {
	START = 1, WRITE, JUMP_FORWARD, JUMP_BACK, ENTER_KEY,SAVE_CHANGES, QUIT = 0};

// class
class Form {
public:
	Form();
	~Form();
	bool capture_value();
	bool capture_value(Performance _Performance);
	auto get_value() -> Performance;
private:
	Performance performance;
	Field *field;
	short activeField;
	short cursor;
	Action action;
	int y, x;
	WINDOW *window;
	PANEL  *panel;
	void create();
	void draw();
	void show();
	void hide();
	bool capture();
};

#endif
