all:
	g++ \
		../lib/utilities/src/files.cpp \
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
		src/main.cpp \
	-o bin/commander \
		-std=gnu++20 -lpanel -lncurses -lasound;
