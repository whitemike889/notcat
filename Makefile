CC = gcc
CFILES = notlib/dbus.c notlib/note.c notlib/queue.c main.c fmt.c
CFLAGS = -Wall -Werror -pthread -O2
DEPS = gio-2.0 gobject-2.0 glib-2.0
LIBS = $(shell pkg-config --libs ${DEPS})
INCLUDES = $(shell pkg-config --cflags ${DEPS})

default: ${MAIN_SRCS}
	${CC} -o notcat ${LIBS} ${CFILES} ${DEFINES} ${CFLAGS} ${INCLUDES}

clean:
	rm -f notcat

notlib.o: notlib/dbus.c notlib/note.c notlib/queue.c notlib/notlib.h
