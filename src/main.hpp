#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <panel.h>

#include "common/common.hpp"
#include "common/matroska.hpp"
#include "graphics/printing.hpp"
#include "graphics/orchestra.hpp"
#include "midi/keyboards.hpp"
#include "graphics/printing.hpp"
#include "graphics/form.hpp"
#include "graphics/windows.hpp"
#include "graphics/ncurses.hpp"
#include "graphics/screen.hpp"
#include "graphics/elements/window.hpp"
#include "common/common.hpp"
#include "common/string.hpp"
#include "common/matroska.hpp"
#include "data/tables.hpp"
#include "data/catalog.hpp"
#include "data/playlist.hpp"
#include "utilities/src/files.hpp"
#include "graphics/colors.hpp"

enum HotKeysMode { VARIATIONS, FAVOURITES };
