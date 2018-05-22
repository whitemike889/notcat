# notcat

It's for when you just want something to print what desktop notifications have
been sent to it.  Much like statnot, but even simpler (and significantly zippier).

Notlib depends on gio/gobject/glib (in particular, the `dbus-glib` package on
arch linux, or equivalents in other environments) for its dbus implementation and
main loop; the intent is to keep these dependencies as small as possible to make
it simple to swap them out if desired.

DISCLAIMER: notcat is in early stages, and as of now, doesn't even fully
implement the desktop notification API.  In particular, it has no concept of a
replacement notification, and signals are unimplemented.

## Formatting

Notcat is configurable via a format string (much like the standard `date`
command).  It takes a single (optional) argument -- the format.  Supported
format sequences are:

```
%i  id
%a  app name
%s  summary
%B  body
%e  expiration timeout
%u  urgency
```

The string `%%` produces a single `%`.

Planned future sequences are:

```
%(H:key)  arbitrary hints
%(A:key)  actions (these will probably be exposed in other ways as well)
%c	  category
```

Notcat supports a "format" hint in notifications, which can be used to
temporarily override the global format string.

In addition to the overall format, notcat will (in the future) support a number
of flags for processing markup and hyperlinks in the notification body, rather
than simply not supporting them (as it does now) or stripping them
indiscriminately (as it will soon).

## onnotify and onclose

Notcat will support optional `--onnotify=[cmd]` and `--onclose=[cmd]` flags,
which will allow users to specify arbitrary processes to spawn upon receipt
and expiration of notifications.  Notcat will pass the filled-in value of its
format string as the arguments to those processes.

`--onnotify` will default to `echo` (and an explicit value of `echo` will be
short-circuited to notcat's internal printing logic), and `--onclose` will
default to nothing at all.  This matches notcat's current behavior.

Details regarding splitting and quoting the arguments to spawned processes are
yet to be designed.

## notlib

Notcat is built on top of the notlib library, which handles the ownership and
management of notifications.  Notlib is intended to be used directly in cases
where the `--onnotify` flag and friends are insufficient.

The notlib API is extremely simple.  There is a single struct,

```c
typedef struct {
    void (*notify)  (const Note *);
    void (*close)   (const Note *);
    void (*replace) (const Note *);
} NoteCallbacks;
```

Users of notlib build this struct of callback functions, and pass it as the
single argument to `notlib_run()`, which runs for the duration of the program.
Notlib owns the lifetime of the `Note *`s; to free or modify them is an error.

The thread-safety of notlib is as of yet undefined.

## TODO, fixes, roadmap

 - finish notlib
    - note replacement
    - thread safety guarantee
    - what if callbacks take a long time?
    - generality -- remove all notcat-specific references
    - resolve all `TODO`s and `FIXME`s
    - signals support
    - manual closing support
    - actions support
 - improve notlib/notcat makefile/header separation
 - more format sequences
 - markup "support", as far as it will go
 - `--onnotify` and `--onclose` (`--onreplace`?)
