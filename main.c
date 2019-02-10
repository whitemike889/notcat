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

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "notlib/notlib.h"
#include "notcat.h"

/**
 * TODO: args for:
 *  - onnotify
 *  - onclose
 *  - hreffmt
 *  - markupfmt
 */

static char *default_fmt_string_opt[] = {"%s", NULL};
static char **fmt_string_opt = default_fmt_string_opt;

void notcat_getopt(int argc, char **argv) {
    if (argc > 1) {
        fmt_string_opt = malloc (sizeof(char *) * argc);
        int i;
        for (i = 0; i < argc - 1; i++) {
            fmt_string_opt[i] = argv[i+1];
        }
        fmt_string_opt[i] = NULL;
    }
}

static void print_note(const NLNote *n) {
    buffer *buf = new_buffer(BUF_LEN);

    char *fmt_override = NULL;
    if (nl_get_string_hint(n, "format", &fmt_override)) {
        fmt_note_buf(buf, fmt_override, n);
        free(fmt_override);
        goto print_buf;
    }

    char **fmt_string;
    for (fmt_string = fmt_string_opt; *fmt_string; fmt_string++) {
        fmt_note_buf(buf, *fmt_string, n);
        if (fmt_string[1])
            put_char(buf, ' ');
    }

print_buf:
    put_char(buf, '\n');
    char *fin = dump_buffer(buf);
    fputs(fin, stdout);
    free(fin);
    fflush(stdout);
}

static uint32_t rc = 0;

void inc_print(const NLNote *n) {
    ++rc;
    print_note(n);
}

void dec(const NLNote *n) {
    if (--rc == 0) {
        printf("\n");
        fflush(stdout);
    }
}

int main(int argc, char **argv) {
    notcat_getopt(argc, argv);

    NLNoteCallbacks cbs = {
        .notify = inc_print,
        .close = dec,
        .replace = print_note
    };
    NLServerInfo info = {
        .app_name = "notcat",
        .author = "jpco",
        .version = "0.2"
    };

    notlib_run(cbs, &info);
    return 0;
}
