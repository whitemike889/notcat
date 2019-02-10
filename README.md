# notcat

It's for when you just want something to print what desktop notifications have been sent to it.  Much like statnot, but even simpler (and significantly zippier).

Not quite ready for the limelight.

Notcat is built using the [notlib](https://github.com/jpco/notlib) library.


## Downloading && installing

Notcat uses git submodules.  To properly clone and build:

1. `git clone https://github.com/jpco/notcat && cd notcat`
2. `git submodule init`
3. `git submodule update`
4. `make` (ensure the glib and dbus libraries are installed)
5. `sudo make install`


## Formatting

Notcat is configurable via format strings (much like the standard `date` command).  It accepts any number of format string arguments.

Supported format sequences are:

```
%i          id
%a          app name
%s          summary
%B          body
%e          expiration timeout
%u          urgency
%(h:NAME)   hint by NAME
```

The string `%%` produces a single `%`.

Planned future sequences are:

```
%(A:key)    actions (these will probably be exposed in other ways as well)
%c          category
```

Notcat supports a string-typed "format" hint, which (for that notification only) overrides the format of the notification.

In addition to the overall format, notcat will (in the future) support a number of flags for processing markup and hyperlinks in the notification body, rather than simply not supporting them (as it does now) or stripping them indiscriminately (as it will soon).

## onnotify and onclose

Notcat will support optional `--onnotify=[cmd]` and `--onclose=[cmd]` flags, which will allow users to specify arbitrary processes to spawn upon receipt and expiration of notifications.  Notcat will pass the expanded value of its format strings as the arguments to those processes.

`--onnotify` will default to `echo` (and an explicit value of `echo` will be short-circuited to notcat's internal printing logic), and `--onclose` will default to nothing at all.  This matches notcat's current behavior.

## TODO, fixes, roadmap

 - more format sequences
 - markup "support", as far as it will go
 - `--onnotify` and `--onclose` (`--onreplace`?)
