# PTY Stuff

pty is a portable pseudoterminal interface written in C but works well with
code written other programming languages.  It has the basic functionality of
TCL's `expect` but without the TCL dependency.

See the pty_driver.pm script for commentary on how pty works for coprocessing
(driving applications with other applications with an intermediary terminal).

```
    % pty -h
    Usage: pty [ -d driver -einvVh -t timeout ] -- program [ arg ... ]
    Options
    -------
      -d (cmd) Driver/coprocess for passed program
      -e       Noecho for slave pty's line discipline
      -i       Ignore EOF on stdin
      -n       Non interactive mode
      -v       Verbose
      -t (sec) Delay initial read from stdin for a clean pty setup (default=1)
      -h       This help menu
      -V       Version
```

`pty -d $driver $app` is a cross-language portable `TCL expect`-like interface
for wiring a coprocess's stdin/stdout directly to a pseudoterminal connected to
`$app`.  Because stderr is unaltered by the `$driver`, it is connected to the
master terminal controlling the `pty` process. We can hence use the driver as
a **copilot** for terminal sessions interacting with `$app = $SHELL, screen,
tmux, etc.`.

## Automated population of "common" credentials

If you are dealing with orchestration engines like `ansible` that manage a
secure encrypted-at-rest Vault, and need to supply the password in a simple,
automated fashion without writing it to disk, you can use `pty` to work through
the password delivery to a contolling terminal.  Have a peek at `pty-driver.pl` to
see how I accomplish this for system login prompts, as well as password-
protected ssh private keys.

The trick with cron+ansible is to avoid the need for pty-driver.pl to call
prompt(), by ensuring all of the required credentials are present in the
already-running `pty-agent` daemon. If needed, pipe `sleep 30` to the `pty`
process on your cron'd ansible script to ensure ansible doesn't reject cron's
closed STDIN.

`C`-wise, it's just a little hacking beyond what you see in W. Richard Stevens'
_Advanced Programming in the Unix Environment_. `Perl5`-wise, there's a lot
of stuff going on, but the only module dependencies are `URI`, `IO::Select`,
and `Term::ReadKey`. `Python3-wise` it just depends on the `setproctitle` module.

## LICENSE

1. The licensing on the scripts is the AL-2.0.

2. The licensing on the `C` sources comes from Addison-Wesley's disclaimers on
code (re)use of [APUE](http://www.kohala.com/start/), since this is a derivative
work of Stevens' (now deceased) online-published source code.

## INSTRUCTIONS:

To build the app, edit the Makefile with the right Make.def.* include for
your OS. Make.def.44 (`gcc` builds) is a popular choice, but doesn't work
on all platforms.  It may require some customization to get your build
working, but I've tested on linux and OSX. Should also work without
modification on FreeBSD.

Currently the Makefile defaults to Make.def.clang, which is a debuggable build
(typically with LLDB as the debugger, which has integration with emacs dap pkg).

Then build pty with
```
    % make
```
and if you get a working `pty` executable, run
```
    % make install
```
to install the pty executable and associated scripts to your ~/bin dir.
Do not `make install` as root, it will prevent you from doing so.

### Common Use Cases Involving Driver:

```
    % pty -d pty-driver.pl $SHELL
    % pty -d pty-driver.pl screen
    % pty -d pty-driver.pl -- ansible-playbook ...
```


# RELEASE NOTES:


## Changes with v2.0.3

- GPG support.


## Changes with v2.0.2:

- custom prompt formats

- better federation of git password credentials


## Changes with v2.0.1:

- echo typed input on any prompt that occurred when the slave term has echo_enabled


## Changes with v2.0.0:

- STTY_NAME env var to driver cmd

- /tmp/ptyon-$USER/$(basename $(ttyname 0)) to mark when driver is enabled

- ttyname, echoon, echooff binaries added

- disabled insecure IN_BAND_TOGGLER

- full ootb support for tmux, etc.


## Changes with v1.1.8:

- url automation in pty-driver.pl

- ansible vault automation in pty-driver.pl


## Changes with v1.1.7:

- better non-blocking read implementation

- clears screen during getpw calls (screen + tmux support!)


## Changes with v1.1.6:

- added isatty exectuable.


## Changes with v1.1.4:

- default to clang debug compiles.


## Changes with v1.1.3:

- reintroduced sysread for better performance.

- code comments review

- better flexibility for non-tty on master pty's STDIN (eg cron usage)


## Changes with v1.1.2:

- cleaned up a few regressions in v1.1.1 related to screen clears.

- echo_enabled no longer takes an argument, and it's only useful when working
  directly with a shell (multiplexers provide their own pty's that we can't
  interface with directly).


## Changes with v1.1.1:

- cosmetic restucturing for non-self regex matching in pty-driver.pl


## Changes with v1.1.0:

- pty-pw-driver.pl split into pty_driver.pm and pty-driver.pl

- API perldoc documentation for pty-driver.pl


## Changes with v1.0.1:

- pty-pw-driver.pl supports non-terminal intput on STDERR.

- pty-pw-driver.pl can be pulled in as a perl4-ish lib using do() or require().


## Changes with v1.0.0:

- pty dups STDIN instead of STDOUT to STDERR for drivers invoked with -d option.
