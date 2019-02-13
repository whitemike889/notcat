# `notcat`

For when you just want something to print, or run, whenever notifications arrive.  Much like statnot, but even simpler (and significantly zippier).

Not ready for the limelight just yet.

Notcat is built using the [notlib](https://github.com/jpco/notlib) library.


## Downloading && installing

Notcat uses git submodules.  To properly clone and build:

1. `git clone https://github.com/jpco/notcat && cd notcat`
2. `git submodule init`
3. `git submodule update`
4. `make` (ensure the glib and dbus libraries are installed for notlib)
5. `sudo make install`


## Running notcat

```
Usage:
  notcat [-h|--help]
  notcat [-se] [--onnotify=<command>] [--onclose=<command>] [--] [format strings]...

Options:
  --onnotify=<command>  Command to run on each notification created (default: echo)
  --onclose=<command>   Command to run on each notification closed
  -s, --shell           Execute commands in a subshell
  -e, --env             Pass notifications to commands in the environment
  -h, --help            This help text
  --                    Stop flag parsing
```

## onnotify and onclose

Notcat can be execute arbitrary commands when notifications are received and closed via the `--onnotify` and `--onclose` flags, respectively.

At present, the commands invoked by `--onnotify` and `--onclose` are blocking -- for notcat, as well as anything that sends a notification while the commands are running.

Notcat has built-in printing behavior, which it will use if `--onnotify=echo` and not `-s` (the default).  It will print each notification as it arrives, formatted using (if set) the notification's string-valued "format" hint, or the format args set on invocation.

The built-in `echo` includes additional formatting as well:

 - It replaces newlines in the notification body with spaces, to keep notifications printed at one line each.

 - (Soon) It will strip markup from notification body text.

 - (Less soon) It will, based on a set of flags, format notification body text.  This should allow nicer piping into terminals, status bars, etc.

By default, notcat's `--onclose` behavior is to simply print a newline when zero notifications are left open.


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

The `%(h:NAME)` format sequence can currently only access hints of type int, byte, boolean, or string.

Planned future sequences are:

```
%(A:key)    actions (these will probably be exposed in other ways as well)
%c          category
```

When the `-e` flag is *not* set, format arguments are filled-in and passed 1:1 to executed commands.  For example, when a notification is sent to notcat executed as

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


## Environment variables

When the `-e` flag is set, notcat will not pass its format flags to its commands, instead setting a number of environment variables which the commands can use.  This may be more convenient for people who write scripts to be executed by notcat.

Environment variables currently sent to commands  when the `-e` flag is set are:

```
NOTE_ID
NOTE_APP_NAME
NOTE_SUMMARY
NOTE_BODY
NOTE_URGENCY
NOTE_TIMEOUT
```

Generic hints are not yet supported in environment variables.

Example usage:

```
$ cat ./post.sh
#!/bin/sh

cat <<EOF
received notification $NOTE_ID from $NOTE_APP_NAME:
$NOTE_SUMMARY

$NOTE_BODY
EOF

$ notcat -e --onnotify=./post.sh
```


## TODO

 - more format sequences and environment variables
 - markup "support", as far as it will go
 - 'notcli', for interacting with notcat "out-of-band"
