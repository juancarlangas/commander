#ifndef FIELDS_H
#define FIELDS_H

#include "../common/common.h"

enum Type {STRING, BANK, VALUE};

//Campos
class Field {
public:
	void create(const char*, 
				const int, const int, const int, const int, 
				const Type, const short);
	void draw();
	void show();
	void hide();
	void get_cursor();
	bool process_key(int);
	void get_string(char []);
	void set_string(const char *);
	void set_string(const char);
	void set_string(const short);
private:
	Type type;
	short limit;
	char cadena[LONG_STRING];
	short nCaracter;
	WINDOW 	*wCaja, *wEspacio;
	PANEL	*pCaja, *pEspacio;
};

#endif
