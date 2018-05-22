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
 *
 * GDBus logic has been liberally adapted from
 * https://github.com/dunst-project/dunst/blob/master/src/dbus.c
 *
 * Desktop notifications API details can be found at
 * https://developer.gnome.org/notification-spec/
 */

#include <glib.h>
#include <stdio.h>

#include "notlib.h"

typedef struct qn {
    Note *n;
    gint64 exp;

    struct qn *prev;
    struct qn *next;
} qnode;

static qnode *note_queue_start = NULL;
static qnode *note_queue_end = NULL;

static void dequeue_note(qnode *qn) {
    if (qn->prev != NULL) {
        qn->prev->next = qn->next;
    } else {
        note_queue_start = qn->next;
    }

    if (qn->next != NULL) {
        qn->next->prev = qn->prev;
    } else {
        note_queue_end = qn->prev;
    }

    if (callbacks.close != NULL)
        callbacks.close(qn->n);

    free_note(qn->n);
    free(qn);
}

// Wake up and scan the queue for things to do!
static gboolean wakeup_queue(gpointer p) {
    gint64 curr_time = (g_get_monotonic_time() / 1000);

    qnode *qn;
    qnode *qnext;
    for (qn = note_queue_start; qn; qn = qnext) {
        qnext = qn->next;
        if (qn->exp <= curr_time)
            dequeue_note(qn);
    }

    return FALSE;
}

// Add a note to the queue, and set a new timeout
extern void enqueue_note(Note *n) {
    qnode *new_qn = malloc(sizeof(qnode));
    // literally DO we care about running out of memory?
    new_qn->n = n;

    int32_t timeout_millis = note_timeout(n) * 1000;
    new_qn->exp = (g_get_monotonic_time() / 1000) + timeout_millis;
    new_qn->next = NULL;

    if (note_queue_start == NULL) {
        note_queue_start = new_qn;
        new_qn->prev = NULL;
        note_queue_end = new_qn;
    } else {
        note_queue_end->next = new_qn;
        new_qn->prev = note_queue_end;
        note_queue_end = new_qn;
    }

    g_timeout_add(timeout_millis, wakeup_queue, NULL);
}
