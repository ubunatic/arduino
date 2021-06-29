#!/usr/bin/env bash
# Script to safely install an Arduino library from a zip file using the following steps.
#
# 1. Allow unsafe_install from zip files.
# 2. Install the library from a given zip file.
# 3. Reset the unsafe_install state to what it was before
#    running the script; even if the install failed.
#

set -e

UNSAFE_KEY="library.enable_unsafe_install"
UNSAFE_VALUE=""

fail() { echo "ERROR: $*" 1>&2; exit 1; }
log()  { echo "INFO: $*" 1>&2; }

get_unsafe_value() {
    log "reading $UNSAFE_KEY"
    arduino-cli config dump --format json | jq ".$UNSAFE_KEY" 2> /dev/null || echo "false"
}

set_unsafe_value() {
    log "setting $UNSAFE_KEY=$1"
    case "$1" in
    true|false) arduino-cli config set $UNSAFE_KEY "$1" ||
                fail "failed to set $UNSAFE_KEY=$1";;
    *)          fail "unsafe value must be 'true' or 'false'";;
    esac
}

install_lib() {
    log "installing '$1'"
    arduino-cli lib install --zip-path "$1" ||
    fail "failed to install arduino library from zip file: '$1'"
}

UNSAFE_VALUE="$(get_unsafe_value)"
log "setting up deferred config restore"
trap "set_unsafe_value '$UNSAFE_VALUE'" EXIT

if test $# -eq 0
then libs=*.zip; log "installing from local zip files: $libs"
else libs="$*";  log "installing from zip files arguments: $libs"
fi
set_unsafe_value true
for f in $libs; do install_lib "$f"; done
