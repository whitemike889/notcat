CC = gcc
SRCS = notcat.c note.c fmt.c
CFLAGS = -Wall -Werror -pthread -O2
LIBS = -lgio-2.0 -lgobject-2.0 -lglib-2.0
INCLUDES = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
DEFINES =

default: ${SRCS}
	${CC} -o notcat ${SRCS} ${DEFINES} ${CFLAGS} ${INCLUDES} ${LIBS}

clean:
	rm -f notcat
