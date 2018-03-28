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

#ifndef NOTCAT_H
#define NOTCAT_H

#include "notlib/notlib.h"

/*
 * To do:
 *  - Customizable (basic) formatting (via flag or env var)
 *  - Expiration/signal(s)
 *  - Customizable markup translation, incl. links (how?)
 *  - Actions, properly (how?)
 *  - arbitrary hints (probably how we do fg/bg color?)
 *  - Persistence?? Is that even possible in this type of UI?
 *
 * hints to-do:
 *  - format - overrides the format flag of the server (neat!)
 *  - category - as per the standard (e.g., for different icons)
 */

#define VERSION 0.1

/* fmt.c */

extern char *fmt_string_opt;

extern char *str_urgency(enum Urgency);
extern void print_note(const Note *);

#endif
