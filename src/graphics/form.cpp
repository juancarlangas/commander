#include <stdlib.h>

#include "form.hpp"

#include "../common/common.hpp"
#include "colors.hpp"

Form::Form()
{
	getmaxyx(stdscr, y, x);

	window	= newwin(y * 160 / 200, x * 120 / 200, y * 30 / 200, x * 40 / 200);	
	panel 	= new_panel(window);

	wattron(window, A_BOLD);
	box(window, 0, 0);
	mvwprintw(window, 0, 20, " Editar song ");

	field[TITLE].create(	"Title", 3, x * 110 / 200, y * 40 / 200, x * 45 / 200, 
								STRING, LONG_STRING);
	field[ARTIST].create(	"Artist", 3, x * 110 / 200, y * 65 / 200, x * 45 / 200,
								STRING, LONG_STRING);
	field[GENRE].create(	"Genre", 3, x * 50 / 200, y * 90 / 200, x * 45 / 200,
								STRING, 35);
	field[SECTION].create(	"Section", 3, x * 38 / 200, y * 90 / 200, x * 96 / 200,
									STRING, 35);
	field[TYPE].create(		"Type", 3, x * 20 / 200, y * 90 / 200, x * 135 / 200,
									STRING, 35);
	field[KEYWORDS].create(	"Keywords", 5, x * 110 / 200, y * 110 / 200, x * 45 / 200,
									STRING, LONG_STRING);
	field[BNK].create(		"Bnk1", 3, x * 10 / 200, y * 144 / 200, x * 130 / 200,
									BANK, 1);
	field[NUM].create(		"Num1", 3, x * 12 / 200, y * 144 / 200, x * 142 / 200,
									VALUE, 3);
}

////////////////////////////////// capture_value() ////////////////////////////////
bool Form::capture_value()
{
	// Initialize
		sprintf(row.title, "");
		sprintf(row.artist, "");
		sprintf(row.genre, "");
		sprintf(row.section, "");
		sprintf(row.keywords, "");
		sprintf(row.type, "");
		row.bnk = 'A';
		row.num = 0;

	show();
		field[TITLE].set_string(row.title);
		field[ARTIST].set_string(row.artist);
		field[GENRE].set_string(row.genre);
		field[SECTION].set_string(row.section);
		field[TYPE].set_string(row.type);
		field[KEYWORDS].set_string(row.keywords);
		field[BNK].set_string(row.bnk);
		field[NUM].set_string(row.num);
		
		bool success = capture();
	hide();

	return success;
}

////////////////////////////////// capture_value(System) ////////////////////////////////
bool Form::capture_value(System get)
{
	row = get;
	
	show();
		field[TITLE].set_string(row.title);
		field[ARTIST].set_string(row.artist);
		field[GENRE].set_string(row.genre);
		field[SECTION].set_string(row.section);
		field[TYPE].set_string(row.type);
		field[KEYWORDS].set_string(row.keywords);
		field[BNK].set_string(row.bnk);
		field[NUM].set_string(row.num);
		
		bool success = capture();
	hide();

	return success;
}
////////////////////////////////////// capture /////////////////////////
bool Form::capture()
{
	char word[LONG_STRING];

	activeField = TITLE;
	field[activeField].get_cursor();

	bool success = false, next = true;

	int key = getch();
	do { // ESCAPE or ENTER
		switch (key) {
			case 9: // JUMP_FORWARD:
				activeField = (activeField + 1) % 8;
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
				if ((key == 32) || (key >= 44 && key <= 122) || (key == 263)) // WRITE
					if (field[activeField].process_key(key) == true) {
						field[activeField].get_string(word);
						switch (activeField) {
							case TITLE:
								sprintf(row.title, "%s", word);
								break;
							case ARTIST:
								sprintf(row.artist, "%s", word);
								break;
							case GENRE:
								sprintf(row.genre, "%s", word);
								break;
							case SECTION:
								sprintf(row.section, "%s", word);
								break;
							case KEYWORDS:
								sprintf(row.keywords, "%s", word);
								break;
							case TYPE:
								sprintf(row.type, "%s", word);
								break;
							case BNK:
								row.bnk = word[0];
								break;
							case NUM:
								row.num = atoi(word);
								break;
						}
					}
		}
		if (next == true)
			key = getch();
	} while (next == true);

	return success;
}

//////////////////////////////////////// get_value //////////////////////////////
System Form::get_value()
{
	return row;
}
////////////////////////////////////// show /////////////////////////////////////////////
void Form::show()
{
	show_panel(panel);
	curs_set(true);

	for (short w = TITLE; w <= 9; w++)
		field[w].show();

	::update_panels();
	::doupdate();
}

////////////////////////////////// hide ////////////////////////////////////////
void Form::hide()
{
	for (short w = TITLE; w <= 9; w++)
		field[w].hide();

	curs_set(false);
	hide_panel(panel);
	
	::update_panels();
	::doupdate();
}
