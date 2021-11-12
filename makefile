all:
	g++ \
		ncurses.c \
		fields.cpp \
		form.cpp \
		channelForm.cpp \
		files.cpp \
		databases.cpp \
		string.c \
		tables.c \
		screen.c \
		digits.c \
		printing.c \
		math.c \
		keyboards.cpp \
		audio.c \
		communicator.cpp \
		commander.cpp \
	-o /usr/local/bin/commander \
		-std=c++98 -lpanel -lncurses -lasound;

