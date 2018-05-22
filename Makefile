CC = gcc
CFILES = notlib/dbus.c notlib/note.c notlib/queue.c main.c fmt.c
CFLAGS = -Wall -Werror -pthread -O2
LIBS = -lgio-2.0 -lgobject-2.0 -lglib-2.0
INCLUDES = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
DEFINES =

default: ${MAIN_SRCS}
	${CC} -o notcat ${CFILES} ${DEFINES} ${CFLAGS} ${INCLUDES} ${LIBS}

clean:
	rm -f notcat

notlib.o: notlib/dbus.c notlib/note.c notlib/queue.c notlib/notlib.h
