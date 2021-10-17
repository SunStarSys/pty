#!/usr/bin/env perl
#
# pty driver customized for SYSTEM password and SSH passphrase prompt
# SPDX License Identifier: Apache License 2.0

use strict;
use warnings FATAL => 'all';
use IPC::Open2;
use URI;

use lib "$ENV{HOME}/bin";
use pty_driver;

# keep 'use strict' happy (these are the two global vars we need
# from pty_driver.pm):

our $PREFIX_RE;
our $NSM;

drive {
  # this is the actual running program where user-customizable
  # code changes (to test $_) go.  Returns true if we handled
  # the contents of $_, false otherwise.

  if (m!\(yes/${NSM}no\)\?! or /'yes' or ${NSM}'no'/) {
    # we always err on the side of caution,
    # but this can be customized differently.
    write_slave "no\n";
  }
  elsif (/^$PREFIX_RE\Q(R)eject, accept (t)emporarily ${NSM}or accept (p)ermanently?/m) {
    # this is a typical ssh unkown-host-key prompt.
    # we do not want to be interrupted for automation,
    # but we also don't want to be connecting without manual
    # verification for our own protection, typically carried
    # out by toggling the driver off temporarily first.
    write_slave "r\n";
  }
  elsif (/^$PREFIX_RE\botp-md5 (\d+) (\w+)/m and not echo_enabled) {
    my $pid = open2 my $out, my $in, "ortcalc $1 $2 2>&-";
    print $in getpw("OTP");
    close $in;
    write_slave <$out>;
    waitpid $pid, 0;
  }
  elsif (/^$PREFIX_RE\bUsername for '([^']+)':/m) {
    write_slave getpw($1, 0, "Username");
  }
  elsif (/^$PREFIX_RE\b[Pp]assword for '([^']+)':/m and not echo_enabled) {
    write_slave getpw($1);
  }
  elsif (/^$PREFIX_RE\bEnter the [Pp]assword for/m and not echo_enabled) {
    write_slave getpw("1Password");
  }
  elsif(/^$PREFIX_RE\[ERROR\].* 401 : Unauthorized/m) {
    # skip to reprompt (on above 1Password login failure)
  }
  elsif (/^$PREFIX_RE\b[Vv]ault [Pp]assword[^:\n]*:/m and not echo_enabled) {
    write_slave getpw("Vault");
  }
  elsif (/^$PREFIX_RE[Pp]assword(?: for $ENV{USER})?$NSM:/m and not echo_enabled) {
    write_slave getpw($ENV{USER});
  }
  elsif (/^$PREFIX_RE(?:Enter passphrase for|Bad passphrase, try again for)$NSM /m and not echo_enabled) {
    write_slave getpw("SSH");
  }
  #
  # to extend the functionality of this script, add new elsif blocks
  # here to check for other types of login prompts you receive.
  # Choose the login $type (no spaces!) to mark things appropriately
  # in your getpw($type) call so it will be tracked properly
  # by pty-agent.
  #
  elsif (/^$PREFIX_RE\QSorry,$NSM try again./m) {
    # skip interceding sudo authentication error message
  }
  elsif (m!^$PREFIX_RE\QDo you want$NSM to continue? [Y/n]!m) {
    # accept the default for this apt-get prompt
    write_slave "\n";
  }
  elsif (exists $ENV{MOZILLA}) {
    # use a port to evade this url pattern on the command-line (history!)
    my (%url_cache, $match);
    while (m!\b(https://[\w.-]+/[$URI::uric#]+)!g) {
      $match++;
      next if $url_cache{$1}++;
      my $url = $1;
      my $pw = getpw($url, 1, "Visit in browser [y/N]?");
      system "('$ENV{MOZILLA}' '$url' >/dev/null 2>&1 &)" if $pw =~ /y/i;
      write_master "\r\n";
    }
    return $match;
  }
  else {
    return 0; # not handled by us
  }
  return 1; # handled successfully
}
