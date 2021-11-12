all:
	g++ \
		src/graphics/ncurses.cpp \
		src/graphics/fields.cpp \
		src/graphics/form.cpp \
		src/graphics/channelForm.cpp \
		src/data/files.cpp \
		src/data/databases.cpp \
		src/common/string.cpp \
		src/data/tables.cpp \
		src/graphics/screen.cpp \
		src/graphics/digits.cpp \
		src/graphics/printing.cpp \
		src/common/math.cpp \
		src/midi/keyboards.cpp \
		src/midi/audio.cpp \
		src/midi/communicator.cpp \
		src/commander.cpp \
	-o bin/commander \
		-std=c++98 -lpanel -lncurses -lasound;

