#include <curses.h>
#include <stdlib.h>

#include "Catalog.hpp"
#include "form.hpp"

#include "../common/common.hpp"

Form::Form()/*{{{*/
{
	getmaxyx(stdscr, y, x); // se crea en tiempo real justo para que de tiempo de obtener x y y

	window	= newwin(y * 160 / 200, x * 120 / 200, y * 30 / 200, x * 40 / 200);	
	panel 	= new_panel(window);

	wattron(window, A_BOLD);
	box(window, 0, 0);
	mvwprintw(window, 0, 20, " Editar song ");

	// We create all the fields
	field = new Field[9]();

	field[TITLE].create("Title", 3, x * 110 / 200, y * 40 / 200, x * 45 / 200, 
			STRING, LONG_STRING);
	field[ARTIST].create("Artist", 3, x * 110 / 200, y * 65 / 200, x * 45 / 200, 
			STRING, LONG_STRING);
	field[GENRE].create("Genre", 3, x * 50 / 200, y * 90 / 200, x * 45 / 200, 
			STRING, 35);
	field[SECTION].create("Section", 3, x * 38 / 200, y * 90 / 200, x * 96 / 200,
			STRING, 35);
	field[TYPE].create("Type", 3, x * 20 / 200, y * 90 / 200, x * 135 / 200,
			STRING, 35);
	field[KEYWORDS].create("Keywords", 5, x * 110 / 200, y * 110 / 200, x * 45/200,
			STRING, LONG_STRING);
	field[BNK].create("Bnk1", 3, x * 10 / 200, y * 144 / 200, x * 130 / 200,
			BANK, 1);
	field[NUM].create("Num1", 3, x * 12 / 200, y * 144 / 200, x * 142 / 200,
			VALUE, 3);
	field[SFZ].create("SFZ", 3, x * 50 / 200, y * 160 / 200, x * 45 / 200,
			STRING, LONG_STRING);
	
	wrefresh( window );
	hide_panel( panel );

	for ( int32_t i = 0; i < 9; ++i )
		field[i].hide();
}/*}}}*/

bool Form::capture_value()/*{{{*/
{
	// Initialize
		performance.tagging.title.clear();
		performance.tagging.artist.clear();
		performance.tagging.genre.clear();
		performance.tagging.mood.clear();
		performance.tagging.keyword.clear();
		performance.type.clear();
		performance.program.bnk = 0;
		performance.program.num = 0;

	show();
		field[TITLE].set_string(performance.tagging.title);
		field[ARTIST].set_string(performance.tagging.artist);
		field[GENRE].set_string(performance.tagging.genre);
		field[SECTION].set_string(performance.tagging.mood);
		field[TYPE].set_string(performance.type);
		field[KEYWORDS].set_string(performance.tagging.keyword);
		field[BNK].set_string(static_cast<char>(performance.program.bnk));
		field[NUM].set_string(std::int32_t{ performance.program.num });
		field[SFZ].set_string(performance.sfz_filename);
		
		bool success = capture();
	hide();

	return success;
}/*}}}*/

bool Form::capture_value(Performance _Performance)/*{{{*/
{
	performance = _Performance;
	
	show();
		field[TITLE].set_string(performance.tagging.title);
		field[ARTIST].set_string(performance.tagging.artist);
		field[GENRE].set_string(performance.tagging.genre);
		field[SECTION].set_string(performance.tagging.mood);
		field[KEYWORDS].set_string(performance.tagging.keyword);
		field[TYPE].set_string(performance.type);
		field[BNK].set_string(static_cast<char>(performance.program.bnk)); // We dont add anything here
		field[NUM].set_string(performance.program.num);
		field[SFZ].set_string(performance.sfz_filename);
		
		bool success = capture();
	hide();

	return success;
}/*}}}*/

bool Form::capture()/*{{{*/
{
	char word[LONG_STRING];

	activeField = TITLE;
	field[activeField].get_cursor();

	bool success = false, next = true;

	int key = getch();
	do { // ESCAPE or ENTER
		switch (key) {
			case 9: // JUMP_FORWARD:
				activeField = (activeField + 1) % 9;
				field[activeField].get_cursor();
				break;

			case 353: // JUMP_BACK;
				if (activeField == TITLE)
					activeField = NUM;
				else
					activeField = (activeField - 1) % 8;
				
				field[activeField].get_cursor();
				break;

			case '\n': // ENTER
				success = true;
				next = false;
				break;

			case 27: // ESCAPE
				next = false;
				break;

			default:
				if ((key == 32) || (key >= 44 && key <= 122) || (key == KEY_BACKSPACE)) // WRITE
					if (field[activeField].process_key(key) == true) {
						field[activeField].get_string(word);
						switch (activeField) {
							case TITLE:
								performance.tagging.title = word;
								break;
							case ARTIST:
								performance.tagging.artist = word;
								break;
							case GENRE:
								performance.tagging.genre = word;
								break;
							case SECTION:
								performance.tagging.mood = word;
								break;
							case KEYWORDS:
								performance.tagging.keyword = word;
								break;
							case TYPE:
								performance.type = word;
								break;
							case BNK:
								performance.program.bnk = static_cast<std::int16_t>(word[0] - 65);
								break;
							case NUM:
								performance.program.num = std::stoi(word);
								break;
							case SFZ:
								performance.sfz_filename = word;
								break;
						}
					}
		}
		if (next == true)
			key = getch();
	} while (next == true);

	return success;
}/*}}}*/

auto Form::get_value() -> Performance/*{{{*/
{
	return performance;
}/*}}}*/

void Form::show()/*{{{*/
{
	show_panel( panel );
	curs_set(true);

	for (short w = TITLE; w < 9; w++)
		field[w].show();

	::update_panels();
	::doupdate();
}/*}}}*/

void Form::hide()/*{{{*/
{
	for (short w = TITLE; w < 9; w++)
		field[w].hide();

	curs_set(false);
	hide_panel(panel);
	
	::update_panels();
	::doupdate();
}/*}}}*/

Form::~Form()
{
	delete [] field;
}
