# `notcat`

It's for when you just want something to print whatever desktop notifications have been sent to it.  Much like statnot, but even simpler (and significantly zippier).

Not quite ready for the limelight.

Notcat is built using the [notlib](https://github.com/jpco/notlib) library.


## Downloading && installing

Notcat uses git submodules.  To properly clone and build:

1. `git clone https://github.com/jpco/notcat && cd notcat`
2. `git submodule init`
3. `git submodule update`
4. `make` (ensure the glib and dbus libraries are installed for notlib)
5. `sudo make install`


## Format strings

Notcat is configurable via format strings (much like the standard `date` command).  It accepts any number of format string arguments.

Supported format sequences are:

```
%%          '%'
%i          id
%a          app name
%s          summary
%B          body
%e          expiration timeout
%u          urgency
%(h:NAME)   hint by NAME
```

Planned future sequences are:

```
%(A:key)    actions (these will probably be exposed in other ways as well)
%c          category
```

In addition to the overall format, notcat will (in the future) support a number of flags for processing markup and hyperlinks in the notification body, rather than simply not supporting them (as it does now) or stripping them indiscriminately (as it will soon).


## onnotify and onclose

```
--onnotify=[cmd]    Command to run on each notification created
--onclose=[cmd]     Command to run on each notification closed
-s                  If provided, execute commands in a subshell
--                  Stop flag parsing
```

Notcat can be execute arbitrary commands when notifications are opened and closed via the `--onnotify` and `--onclose` flags, respectively.

Format arguments will be filled-in and passed 1:1 to executed commands.  For example, when a notification is sent to notcat executed as

```
$ notcat --onnotify=/bin/echo %s '%u %B'
```

notcat will execute the command as follows:

```
/bin/echo '<summary>' '<urgency> <body>'
```

On the other hand, if the `-s` flag is also passed to the above notcat invocation, it will execute the command as:

```
$SHELL -c /bin/echo notcat '<summary>' '<urgency> <body>'
```

In this case, the command isn't very useful (simply echoing an empty line each time).  Figuring out how shells' `-c` flag works is left as an exercise for the reader. (The 'notcat' in the above is hard-coded, setting the `$0` of the executed subshell.)

At present, the commands invoked by `--onnotify` and `--onclose` are blocking -- for notcat, as well as anything that sends a notification while the commands are running.


### Default behavior

Notcat has built-in printing behavior, which it will use if `--onnotify=echo` and not `-s` (the default).  It will print each notification as it arrives, formatted using (if set) the notification's string-valued "format" hint, or notcat's own format flags.

By default, notcat's `--onclose` behavior is to simply track the number of currently open notifications, and print a newline when zero notifications are left.


## TODO, fixes, roadmap

 - more format sequences
 - markup "support", as far as it will go
