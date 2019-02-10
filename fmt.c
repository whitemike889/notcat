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
 * arbitrary hints  %(h:key)
 * category         %c          - TODO?
 * expire_timeout   %e
 * urgency          %u
 */

static char *default_fmt_string_opt[] = {"%s", NULL};
char **fmt_string_opt = default_fmt_string_opt;

extern char *str_urgency(const enum NLUrgency u) {
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

static void print_hint(const NLNote *n, char *name) {
    int ih;
    unsigned char bh;
    char *sh;

    switch (nl_get_hint_type(n, name)) {
    case HINT_TYPE_INT:
        nl_get_int_hint(n, name, &ih);
        putint(ih);
        break;
    case HINT_TYPE_BYTE:
        nl_get_byte_hint(n, name, &bh);
        putuint(bh);
        break;
    case HINT_TYPE_BOOLEAN:
        nl_get_boolean_hint(n, name, &ih);
        fputs((ih ? "TRUE" : "FALSE"), stdout);
        break;
    case HINT_TYPE_STRING:
        nl_get_string_hint(n, name, &sh);
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
static void print_note_string(const NLNote *n, char *fmt) {
    char *c;
    char state = NORMAL;
    char *body = NULL;

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
                if (body == NULL) {
                    body = (n->body == NULL ? "" : malloc(1 + strlen(n->body)));
                    if (body == NULL) {
                        perror("could not allocate");
                        return;
                    }
                    fmt_body(n->body, body);
                }
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

    if (body != NULL && n->body != NULL)
        free(body);
}

extern void print_note(const NLNote *n) {
    char *fmt_override = NULL;
    if (nl_get_string_hint(n, "format", &fmt_override)) {
        print_note_string(n, fmt_override);
        free(fmt_override);
        return;
    }

    char **fmt_string;
    for (fmt_string = fmt_string_opt; *fmt_string; fmt_string++) {
        print_note_string(n, *fmt_string);
        if (fmt_string[1])
            putchar(' ');
    }

    fputs("\n", stdout);
    fflush(stdout);
}
