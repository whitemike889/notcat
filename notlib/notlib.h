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

#ifndef NOTLIB_H
#define NOTLIB_H

#define VERSION 0.1

#include <stdint.h>

// by default, say we support actions :)
#ifndef ACTIONS
#define ACTIONS 1
#endif

#define FDN_PATH "/org/freedesktop/Notifications"
#define FDN_IFAC "org.freedesktop.Notifications"
#define FDN_NAME "org.freedesktop.Notifications"

/*
 * typedefs
 */

#if ACTIONS
typedef struct {
    char **actions;
    long unsigned int count;
} Actions;
#endif

enum Urgency {
    URG_NONE = -1,
    URG_MIN  =  0,
    URG_LOW  =  0,
    URG_NORM =  1,
    URG_CRIT =  2,
    URG_MAX  =  2
};

/* a bit premature, but for the NotificationClosed signal */
enum CloseReason {
    CLOSE_REASON_EXPIRED = 1,
    CLOSE_REASON_DISMISSED = 2,
    CLOSE_REASON_CLOSED = 3,
    CLOSE_REASON_IDK = 4
};

typedef struct {
    unsigned int id;
    char *appname;
    char *summary;
    char *body;

    int timeout;
#if ACTIONS
    Actions *actions;
#endif
    /* char *dbus_client; */

    /* hints */
    char *format; /* custom format for just this notification */
    enum Urgency urgency;
} Note;

/*
 * functions
 */

/* note.c */
extern Note *new_note(uint32_t,     /* id */
                      char *,       /* app name */
                      char *,       /* summary */
                      char *,       /* body */
#if ACTIONS
                      Actions *,    /* actions */
#endif
                      int32_t,      /* timeout */
                      enum Urgency, /* urgency */
                      char *        /* format (optional) */
                     );

#if ACTIONS
extern void free_actions(Actions *);
#endif

extern void free_note(Note *);

/* dbus.c */

extern void run_dbus(void (*on_notify)(const Note *));

#endif
