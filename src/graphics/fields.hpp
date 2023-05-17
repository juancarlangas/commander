#ifndef FIELDS_H
#define FIELDS_H

#include "../common/common.hpp"
#include <string>

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
	void set_cursor() noexcept;
	bool process_key(int);
	void get_string(char []);
	void set_string(const std::string& _Cadena);
	void set_string(const char);
	void set_string(const std::int32_t &_Number);
	void set_content( const std::string & ) noexcept;
	void set_font_color( const int32_t &color ) noexcept;
	void set_font_width( const std::string_view ) noexcept;
private:
	Type type;
	short limit;
	char cadena[LONG_STRING];
	short nCaracter;
	WINDOW 	*wCaja, *wEspacio;
	PANEL	*pCaja, *pEspacio;
};

#endif
