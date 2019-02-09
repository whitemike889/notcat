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

/*
 * Proposed format syntax (incomplete)
 *
 * id               %i
 * app_name         %a
 * summary          %s
 * body             %B          - also %(B:30) for 'first 30 chars of b'?
 * actions          ???         - TODO - %(A:key)?
 * arbitrary hints  %(H:key) ?
 * category         %c          - TODO?
 * expire_timeout   %e
 * urgency          %u
 */

char *fmt_string_opt = "%s";

extern char *str_urgency(const enum Urgency u) {
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

static void putint(int32_t i) {
    if (i < 0) {
        putchar('-');
        putuint(UINT32_MAX - (uint32_t) i + 1);
    } else {
        putuint(i);
    }
}

static void fmt_body(const char *in, char *out) {
    size_t i;
    char c;
    for (i = 0; (c = in[i]); ++i) {
        switch (c) {
        case '\n':
            out[i] = ' ';
            break;
        default:
            out[i] = c;
        }
    }
    out[i] = '\0';
}

static void print_hint(const Note *n, char *name) {
    int ih;
    unsigned char bh;
    char *sh;

    switch (get_hint_type(n, name)) {
    case HINT_TYPE_INT:
        get_int_hint(n, name, &ih);
        putint(ih);
        break;
    case HINT_TYPE_BYTE:
        get_byte_hint(n, name, &bh);
        putuint(bh);
        break;
    case HINT_TYPE_BOOLEAN:
        get_boolean_hint(n, name, &ih);
        fputs((ih ? "TRUE" : "FALSE"), stdout);
        break;
    case HINT_TYPE_STRING:
        get_string_hint(n, name, &sh);
        fputs(sh, stdout);
        free(sh);
        break;
    default:
        break;
    }
}

#define NORMAL      0
#define PCT         1
#define PCTPAREN    2
#define HINT        3

/* We assume, maybe incorrectly, that printf has OK buffering behavior */
/* TODO: make sure we're unicode-friendly here */
extern void print_note(const Note *n) {
    char *c;
    char state = NORMAL;

    char *body = (n->body == NULL ? "" : malloc(1 + strlen(n->body)));
    if (body == NULL) {
        perror("could not allocate");
        return;
    }

    fmt_body(n->body, body);

    char *fmt_override = NULL;
    char *fmt = fmt_string_opt;
    if (get_string_hint(n, "format", &fmt_override))
        fmt = fmt_override;

    for (c = fmt; *c; c++) {
        switch (state) {
        case HINT: {
            char *c2;
            c++;
            for (c2 = c; *c2 && *c2 != ')'; c2++)
                ;
            if (!*c2) {
                printf("%%(h:%s", c);
                c = c2 - 1;
            } else {
                *c2 = '\0';
                print_hint(n, c);
                *c2 = ')';
                c = c2;
                state = NORMAL;
            }
            break;
        }
        case PCTPAREN:
            switch (*c) {
            case 'h':
                if (c[1] == ':') {
                    state = HINT;
                    break;
                }
            default:
                printf("%%(%c", *c);
                state = NORMAL;
                break;
            }
            break;
        case PCT:
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
                putint(n->timeout);
                break;
            case 'u':
                fputs(str_urgency(n->urgency), stdout);
                break;
            case '%':
                putchar('%');
                break;
            case '(':
                state = PCTPAREN;
                break;
            default:
                putchar('%');
                putchar(*c);
            }
            if (state == PCT) {
                state = NORMAL;
            }
            break;
        case NORMAL:
            switch (*c) {
            case '%':
                state = PCT;
                break;
            default:
                printf("%c", *c);
            }
            break;
        }
    }

    switch (state) {
    case PCT:
        fputs("%", stdout);
        break;
    case PCTPAREN:
        fputs("%(", stdout);
        break;
    }

    fputs("\n", stdout);
    fflush(stdout);

    if (fmt_override != NULL)
        free(fmt_override);

    if (n->body != NULL)
        free(body);
}
