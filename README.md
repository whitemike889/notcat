# notcat

It's for when you just want something to print what desktop notifications have
been sent to it.  Much like statnot, but even simpler (and significantly more
performant).

Depends on gio/gobject/glib for its dbus implementation; the intent is to
shrink these dependencies as much as possible to make it simple to swap them
out if desired.

DISCLAIMER: notcat is in early stages, and as of now, doesn't even fully
implement the desktop notification API.  In particular, it doesn't (really)
understand what replacement IDs are, and it has no concept of a notification
closing (as far as it knows, a notification is a point-in-time kind of thing).
It is also missing signals, as well as any notion of actions (though what the
latter will actually look like is still rather up in the air).

## Formatting

notcat is configurable via a format string (much like the standard `date`
command).  This string is currently not configurable via argv (this is a TODO),
but rather by redefining `FMT_STRING` in `fmt.c`.  Supported format sequences
are:

```
%i  id
%a  app_name
%s  summary
%B  body
%e  expire_timeout
%u  urgency
```

Planned future sequences are:

```
%(H:key)  arbitrary hints
%(A:key)  actions (these will probably be exposed in other ways as well)
%c	  category
```

notcat will also (in the future) support a "format" hint in notifications,
which can be used to temporarily override the global format string.

In addition to the overall format, notcat will (in the future) support a number
of flags for processing markup and hyperlinks in the notification body, rather
than simply not supporting them (as it does now) or stripping them
indiscriminately (as it will soon).
