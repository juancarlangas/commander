#pragma once

#include "databases.hpp"

int load_playlist(System plArray[], const char* file_name);
void save_playlist(System plArray[], const int plRows, const char* file_name);
