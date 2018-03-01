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
                printf("%u", n->id);
                break;
            case 'a':
                printf("%s", (n->appname == NULL ? "" : n->appname));
                break;
            case 's':
                printf("%s", (n->summary == NULL ? "" : n->summary));
                break;
            case 'B':
                printf("%s", body);
                break;
            case 'e':
                printf("%d", n->timeout);
                break;
            case 'u':
                printf("%s", str_urgency(n->urgency));
                break;
            case '%':
                printf("%%");
                break;
            default:
                printf("%%%c", *c);
            }
            pct = 0;
        } else if (*c == '%') {
            pct = 1;
        } else {
            printf("%c", *c);
        }
    }
    printf((pct ? "%%\n" : "\n"));
    fflush(stdout);
}
