CC = gcc

CSRC = fmt.c main.c
HSRC = notcat.h
OBJS = fmt.o main.o

CFLAGS = -Wall -Werror -O2

DEPS     = gio-2.0 gobject-2.0 glib-2.0
LIBS     = $(shell pkg-config --libs ${DEPS})
INCLUDES = $(shell pkg-config --cflags ${DEPS})

notcat 		: ${OBJS} libnotlib.a
	${CC} -o notcat ${DEFINES} ${CFLAGS} ${CSRC} -L./notlib -lnotlib ${LIBS} ${INCLUDES}

libnotlib.a	:
	$(MAKE) static -C notlib

install		: notcat
	mkdir -p /usr/local/bin
	cp notcat /usr/local/bin

clean		:
	$(MAKE) clean -C notlib
	rm *.o notcat
