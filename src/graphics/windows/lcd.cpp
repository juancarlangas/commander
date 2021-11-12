#include "lcd.h"
#include "../global.h"

void LCD::draw()
{
	window = newwin(4, xRes * 98 / 200, yRes * 0 / 001, xRes * 50 / 200);
}