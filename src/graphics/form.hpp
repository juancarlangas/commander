#ifndef FORM_H
#define FORM_H

#include "graphics/fields.hpp"
#include "common/common.hpp"
#include "data/databases.hpp"

// Encuadre
#define TITLE		0
#define ARTIST  	1
#define GENRE  		2
#define SECTION 	3
#define TYPE	 	4
#define KEYWORDS	5
#define BNK 		6
#define NUM 		7

enum Action {	
		START = 1,
		WRITE,
		JUMP_FORWARD,
		JUMP_BACK,
		ENTER_KEY,
		SAVE_CHANGES,
		QUIT = 0	};

// class
class Form {
public:
	Form();
	bool capture_value();
	bool capture_value(System);
	System get_value();
private:
	System row;
	Field field[10];
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
