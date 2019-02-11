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
#include <string.h>

#include "notlib/notlib.h"
#include "notcat.h"

/**
 * TODO: args for:
 *  - hreffmt
 *  - markupfmt
 */

static char *on_notify_opt = NULL;
static char *on_close_opt = NULL;

void notcat_getopt(int argc, char **argv) {
    char **fmt_opt = malloc (sizeof(char *) * argc);
    size_t fo_idx = 0;

    int av_idx;
    int skip = 0;
    for (av_idx = 1; av_idx < argc; av_idx++) {
        if (!skip && !strncmp("--onnotify=", argv[av_idx], 11)) {
            if (argv[av_idx][11] != '\0')
                on_notify_opt = argv[av_idx] + 11;
            continue;
        } else if (!skip && !strncmp("--onclose=", argv[av_idx], 10)) {
            if (argv[av_idx][10] != '\0')
                on_close_opt = argv[av_idx] + 10;
            continue;
        } else if (!skip && !strcmp("-s", argv[av_idx])) {
            shell_run_opt = 1;
            continue;
        } else if (!skip && !strcmp("--", argv[av_idx])) {
            skip = 1;
            continue;
        }
        fmt_opt[fo_idx++] = argv[av_idx];
    }

    if (fo_idx > 0) {
        fmt_string_opt_len = fo_idx;
        fmt_string_opt = fmt_opt;
    } else free(fmt_opt);
}

static uint32_t rc = 0;

void inc(const NLNote *n) {
    ++rc;
    if (!on_notify_opt || (!strcmp(on_notify_opt, "echo") && !shell_run_opt)) {
        print_note(n);
    } else {
        run_cmd(on_notify_opt, n);
    }
    fflush(stdout);
}

void dec(const NLNote *n) {
    --rc;
    if (!on_close_opt) {
        if (rc == 0) {
            printf("\n");
        }
    } else {
        run_cmd(on_close_opt, n);
    }
    fflush(stdout);
}

int main(int argc, char **argv) {
    notcat_getopt(argc, argv);

    NLNoteCallbacks cbs = {
        .notify = inc,
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
