#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "graphics/orchestra.hpp"
#include "midi/keyboards.hpp"
#include "midi/communicator.hpp"
#include "graphics/printing.hpp"
#include "graphics/form.hpp"
#include "graphics/windows.hpp"
#include "graphics/ncurses.hpp"
#include "graphics/screen.hpp"
#include "graphics/window.hpp"
#include "common/common.hpp"
#include "common/string.hpp"
#include "common/matroska.hpp"
#include "data/tables.hpp"
#include "data/databases.hpp"
#include "data/files.hpp"
#include "../../lib/utilities/src/files.hpp"

extern short int x, y;
extern short int displayShowResults, playlistShowResults;

extern Orchestra orquestacion;
