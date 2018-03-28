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
#include <glib.h>

#include "notlib.h"

extern Note *new_note(uint32_t id, char *appname, char *summary, char *body,
#if ACTIONS
                      Actions *actions,
#endif
                      int32_t timeout, enum Urgency urgency, char *format) {
    Note *n = g_malloc(sizeof(Note));

    /* TODO: what if this note is replacing another? */
    n->id = id;
    n->appname = appname;
    n->summary = summary;
    n->body    = body;

    n->timeout = timeout;
#if ACTIONS
    n->actions = actions;
#endif

    n->urgency = urgency;
    n->format  = format;

    return n;
}

#if ACTIONS
extern void free_actions(Actions *a) {
    if (!a) return;

    g_strfreev(a->actions);
    g_free(a);
}
#endif

extern void free_note(Note *n) {
    if (!n) return;

    g_free(n->format);
    g_free(n->appname);
    g_free(n->summary);
    g_free(n->body);

#if ACTIONS
    free_actions(n->actions);
#endif

    g_free(n);
}
