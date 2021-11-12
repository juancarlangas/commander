all:
	g++ \
		src/graphics/ncurses.c \
		src/graphics/fields.cpp \
		src/graphics/form.cpp \
		src/graphics/channelForm.cpp \
		src/data/files.cpp \
		src/data/databases.cpp \
		src/common/string.c \
		src/data/tables.c \
		src/graphics/screen.c \
		src/graphics/digits.c \
		src/graphics/printing.c \
		src/common/math.c \
		src/midi/keyboards.cpp \
		src/midi/audio.c \
		src/midi/communicator.cpp \
		src/commander.cpp \
	-o bin/commander \
		-std=c++98 -lpanel -lncurses -lasound;

