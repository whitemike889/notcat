/* Copyright 2018 Jack Conger */

/*
 * This file is part of notcat.
 *
 * notcat is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * notcat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with notcat.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>

#include "notcat.h"

/**
 * TODO: args for:
 *  - onnotify
 *  - onclose
 *  - hreffmt
 *  - markupfmt
 *
 * These should all be behind #DEFINEs when implemented.
 */

void notcat_getopt(int argc, char **argv) {
    if (argc == 2)
        fmt_string_opt = argv[1];
    else if (argc > 1) {
        fprintf(stderr, "usage: %s [fmtstring]\n", argv[0]);
        exit(2);
    }
}

static uint32_t rc = 0;

void inc_print(const Note *n) {
    ++rc;
    print_note(n);
}

void dec(const Note *n) {
    if (--rc == 0)
        printf("\n");
}

int main(int argc, char **argv) {
    NoteCallbacks cbs = { .notify = inc_print, .close = dec, .replace = print_note };

    notcat_getopt(argc, argv);

    notlib_run(cbs);
    return 0;
}
