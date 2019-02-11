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
#include <stddef.h>
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

extern void put_urgency(buffer *buf, const enum NLUrgency u) {
    switch (u) {
    case URG_NONE:
        put_str(buf, "NONE");
        break;
    case URG_LOW:
        put_str(buf, "LOW");
        break;
    case URG_NORM:
        put_str(buf, "NORMAL");
        break;
    case URG_CRIT:
        put_str(buf, "CRITICAL");
        break;
    default:
        put_str(buf, "IDFK");
        break;
    }
}

static void put_uint(buffer *buf, uint32_t u) {
    if (u == 0) {
        put_char(buf, '0');
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
            put_char(buf, chs[idx]);
    }
}

static void put_int(buffer *buf, int32_t i) {
    if (i < 0) {
        put_char(buf, '-');
        put_uint(buf, UINT32_MAX - (uint32_t) i + 1);
    } else {
        put_uint(buf, i);
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

static void put_hint(buffer *buf, const NLNote *n, char *name) {
    int ih;
    unsigned char bh;
    char *sh;

    switch (nl_get_hint_type(n, name)) {
    case HINT_TYPE_INT:
        nl_get_int_hint(n, name, &ih);
        put_int(buf, ih);
        break;
    case HINT_TYPE_BYTE:
        nl_get_byte_hint(n, name, &bh);
        put_uint(buf, bh);
        break;
    case HINT_TYPE_BOOLEAN:
        nl_get_boolean_hint(n, name, &ih);
        put_str(buf, (ih ? "TRUE" : "FALSE"));
        break;
    case HINT_TYPE_STRING:
        nl_get_string_hint(n, name, &sh);
        put_str(buf, sh);
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

extern void fmt_note_buf(buffer *buf, char *fmt, const NLNote *n) {
    char *c;
    char state = NORMAL;
    char *body = NULL;

    for (c = fmt; *c; c++) {
        switch (state) {
        case HINT: {
            char *c2;
            ++c;
            for (c2 = c; *c2 && *c2 != ')'; c2++)
                ;
            if (!*c2) {
                put_str(buf, "%(h:");
                put_str(buf, c);
                c = c2 - 1;
            } else {
                *c2 = '\0';
                put_hint(buf, n, c);
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
                put_str(buf, "%(");
                put_char(buf, *c);
                state = NORMAL;
                break;
            }
            break;
        case PCT:
            switch (*c) {
            case 'i':
                put_uint(buf, n->id);
                break;
            case 'a':
                put_str(buf, (n->appname == NULL ? "" : n->appname));
                break;
            case 's':
                put_str(buf, (n->summary == NULL ? "" : n->summary));
                break;
            case 'B':
                if (body == NULL) {
                    body = (n->body == NULL ? "" : malloc(1 + strlen(n->body)));
                    fmt_body(n->body, body);
                }
                put_str(buf, body);
                break;
            case 'e':
                put_int(buf, n->timeout);
                break;
            case 'u':
                put_urgency(buf, n->urgency);
                break;
            case '%':
                put_char(buf, '%');
                break;
            case '(':
                state = PCTPAREN;
                break;
            default:
                put_char(buf, '%');
                put_char(buf, *c);
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
                put_char(buf, *c);
            }
            break;
        }
    }

    switch (state) {
    case PCT:
        put_char(buf, '%');
        break;
    case PCTPAREN:
        put_str(buf, "%(");
        break;
    }

    if (body != NULL && n->body != NULL)
        free(body);
}

extern char *fmt_note(char *fmt, const NLNote *n) {
    buffer *buf = new_buffer(BUF_LEN);
    fmt_note_buf(buf, fmt, n);
    return dump_buffer(buf);
}
