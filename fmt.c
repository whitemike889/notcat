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
#include <stdio.h>
#include <string.h>

#include "notcat.h"

/*
 * Proposed format syntax (incomplete)
 *
 * id               %i
 * app_name         %a
 * summary          %s
 * body             %B          - also %(B:30) for 'first 30 chars of b'?
 * actions          ???         - TODO - %(A:key)?
 * arbitrary hints  %(H:key) ?  - TODO
 * category         %c          - TODO?
 * expire_timeout   %e
 * urgency          %u
 */

char *fmt_string = "%s";

extern char *str_urgency(enum Urgency u) {
    switch (u) {
        case URG_NONE: return "NONE";
        case URG_LOW:  return "LOW";
        case URG_NORM: return "NORMAL";
        case URG_CRIT: return "CRITICAL";
        default:       return "IDFK";
    }
}

static void putuint(uint32_t u) {
    if (u == 0) {
        putchar('0');
        return;
    }

    char chs[10] = {0, 0, 0, 0, 0};
    size_t idx;
    for (idx = 9; u > 0; idx--) {
        chs[idx] = '0' + (u % 10);
        u /= 10;
    }

    for (idx = 0; idx < 10; idx++) {
        if (chs[idx] != 0)
            putchar(chs[idx]);
    }
}

/* We assume, maybe incorrectly, that printf has OK buffering behavior */
/* TODO: make sure we're unicode-friendly here */
extern void print_note(Note *n) {
    char *c;
    char pct = 0;

    char *body = (n->body == NULL ? "" : strdup(n->body));
    if (body == NULL) {
        perror("could not allocate");
        return;
    }

    char *bp;
    for (bp = body; *bp != '\0'; ++bp)
        if (*bp == '\n') *bp = ' ';

    char *fmt = (n->format == NULL ? fmt_string : n->format);

    for (c = fmt; *c; c++) {
        if (pct) {
            switch (*c) {
            case 'i':
                putuint(n->id);
                break;
            case 'a':
                fputs((n->appname == NULL ? "" : n->appname), stdout);
                break;
            case 's':
                fputs((n->summary == NULL ? "" : n->summary), stdout);
                break;
            case 'B':
                fputs(body, stdout);
                break;
            case 'e':
                if (n->timeout < 0)
                    putchar('-');
                putuint((uint32_t) n->timeout);
                break;
            case 'u':
                fputs(str_urgency(n->urgency), stdout);
                break;
            case '%':
                putchar('%');
                break;
            default:
                putchar('%');
                putchar(*c);
            }
            pct = 0;
        } else if (*c == '%') {
            pct = 1;
        } else {
            printf("%c", *c);
        }
    }

    fputs((pct ? "%%\n" : "\n"), stdout);
    fflush(stdout);

    if (n->body != NULL)
        free(body);
}
