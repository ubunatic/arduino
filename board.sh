#!/usr/bin/env bash

set -o errexit
set -o pipefail

PIO_SOURCE=$HOME/.platformio/penv/bin/activate

if test -e "$PIO_SOURCE" && ! command -v pio
then source $PIO_SOURCE
fi > /dev/null

usage() {
    echo "Arduino Dev Helper: Finds and manages the first arduino device for development.

    Usage: $0 OPTIONS COMMANDS

    Commands:
        list     show all boards
        show     show basic board data of first board
        details  show board detals of first board
        attach   attach first board for development
        address  show system address of first board
        board    show 'fqbn' (fully qualified board name) of first board
        serial   start a serial console to the first board
        reset    stop and reset the first serial console
        compile  compile the current project
        upload   upload project to board
        ardu     convert PlatformIO C++ code to Arduino ino-file
        ide      open converted project in Arduino IDE
        help     show this help

    PlatformIO Commands:
        pio compile  build PlatformIO C++ project
        pio upload   upload PlatformIO C++ project
        pio serial   start PlatformIO serial monitor

    Options
        -C DIR       change to DIR before running (remaining) commands
        -P | --pio   run all commands as 'pio' commands
    "
}

# BOARD allows setting the board name.
BOARD=
# BAUD allows setting the baud rate of the serial console.
BAUD=${BAUD:-9600}
# ADDR allows setting the address of the board.
ADDR=
# SERIAL_PID allows setting the PID of the current serial console process.
SERIAL_PID=

# INO_DIR defines the location of the original Arduino project.
INO_DIR=${INO_DIR:-ardu}

# CPP_DIR defines where to find raw C++ code managed by PlatformIO
CPP_DIR=${CPP_DIR:-src}

# log writes log messages to stderr.
log()  { echo "INFO: $*" 1>&2; }

# debug writes debug messages to stderr.
debug() { if test "$DEBUG"; then echo "DEBUG: $*" 1>&2; fi; }

# fail logs a given error message to stderr and then exits the program.
fail() { echo "ERROR: $*" 1>&2; exit 1; }

# safe_board runs arduino-cli board commands with additional error checks.
safe_board() {
    test $# -gt 0                 || fail "no board command or paramaters specified"
    res=$(arduino-cli board "$@") || fail "board command 'arduino-cli board $*' failed with code=$?"

    # also check output for hidden errors when command failed but exit code was still 0
    if   echo "$res" | grep "arduino-cli board" > /dev/null
    then fail "board command 'arduino-cli board $*' failed (code=0, but printed usage information)"
    fi

    # all is good now and we can release the captured output 🕊
    echo "$res"
}

# board safely runs a board command.
board()    { safe_board "$@"; }

# board_js safely runs a board command using JSON output.
board_js() { safe_board "$@" --format=json | jq -r .; }

# query list all boards as JSON data and queries the data with the given jq expression.
query()    { board_js list | jq -r "$@";  }

# show gets JSON data for the first board.
show() { query ".[0]"; }

# fqbn finds the first board name in the board data.
fqbn() { query ".[0].boards[0].fqbn"; }

# addr finds the first board address in the board data.
addr() { query ".[0].address"; }

set_serial() { set_addr && SERIAL_PID="$(pgrep -f "cat $ADDR" || true)"; }
set_addr()   { ADDR="${ADDR:-$(addr)}"    || fail "failed to set board address"; }
set_board()  { BOARD="${BOARD:-$(fqbn)}"  || fail "failed to set board name"; }
set_ino()    {
    test -n "$INO_DIR"                                 || fail "arduino project dir not set"
    INO_FILE="${INO_FILE:-"$(basename $INO_DIR).ino"}" || fail "failed to set ino-file"
}

# reset stops and resets the serial console.
reset() {
    set_serial
    if test -n "$SERIAL_PID"; then
        log "stopping running serial console process PID=$SERIAL_PID"
        kill $SERIAL_PID || fail "failed to stop serial"
        SERIAL_PID=
    fi
	stty -F $ADDR raw $BAUD
}

# serial starts a new serial console.
serial() {
    set_serial
    set_addr || fail "failed to set board address"
    test -z "$SERIAL_PID" || reset
    log "log starting serial console at $ADDR"
    sleep 2  # wait for the device to be ready
    cat $ADDR
    log "serial console closed"
}

# compile compiles the current project.
compile() { arduino-cli compile; }

# upload uploads the compiled project to the board.
upload()  { set_addr && arduino-cli upload -v -t -p $ADDR; }

# libs reads libraries.txt and installs the listed libs.
libs() {
    test -e libraries.txt || fail "libraries.txt not found"
    libs="$(
        cat libraries.txt  |
        grep -v -e '#\|^$' |  # remove comments
        sed -e 's/ /\\ /g' |  # escape spaces
        tr '\n' ' '           # flatten file
    )"
    bash -c "arduino-cli lib install $libs"
}

# ide starts the Arduino IDE and squelches its noisy logs.
ide() {
    set_ino
    arduino "$INO_FILE" 1>/dev/null 2>/dev/null &
	log "Arduino started in the background." \
        "You may need to reconnect your board" \
        "since the IDE ignores some values in the sketch.json file."
}

# cp_if_exists copies only if it exists at the source location.
cp_if_exists() {
    debug "trying to copy $1 to $2"
    if test -e "$1"; then cp "$1" "$2"; fi || fail "failed to copy $1 to $2"
}

# copies PlatformIO project code to a new arduino project in $INO_DIR
pio2ardu() {
    set_ino
	log "converting cpp code in $CPP_DIR to an arduino project in $INO_DIR"
    test -e "$CPP_DIR"                                 || fail "C++ source dir not set"
	mkdir -p $INO_DIR                                  || fail "failed to create Arduino dir"
	tail -n +2 $CPP_DIR/main.cpp > $INO_DIR/$INO_FILE  || fail "failed to convert C++ code"
    log "copied Arduino code to $INO_DIR/$INO_FILE"
    cp_if_exists libraries.txt $INO_DIR
}

# ENV defines the Platform IO env
ENV=${ENV:-uno}

pio_compile() { pio run -e $ENV; }
pio_upload()  { pio run -e $ENV -t upload; }
pio_serial()  { pio device monitor 2>/dev/null; }
pio_()        {
    pio_compile || fail "failed to compile pio cpp code"
    pio_upload  || fail "failed to upload compiled pio code"
    pio_serial && log "serial monitor stopped" || fail "serial monitor interrupted"
}

main() {
    while test $# -gt 0; do
    cmd="$1"
    shift
    if case "$cmd" in
        -C|cd)            cmd="cd $1"; cd "$1"; shift ;;
        -P|--pio)         PIO=true ;;
        pio)              cmd="pio_$1"; "$cmd"; test $# -eq 0 || shift ;;
        *=*)              export "$cmd" && debug "setting $cmd" ;;
        list|ls)          board list ;;
        details)          set_board && board details -b $BOARD ;;
        attach)           set_board && board attach     $BOARD ;;
        attach-serial)    set_addr  && board attach     $ADDR ;;
        serial)           if $PIO; then pio_serial;  else serial; fi ;;
        compile)          if $PIO; then pio_compile; else compile; fi ;;
        upload)           if $PIO; then pio_upload;  else upload;  fi ;;
        reset)            reset ;;
        ardu)             pio2ardu ;;
        ide)              ide ;;
        lib|libs)         libs ;;
        show|1)           show ;;
        port|addr*)       addr ;;
        fqbn|bn|board)    fqbn ;;
        js)               board_js "$@" && shift $# ;;
        h|-h|--help|help) usage ;;
        *)                fail "unknown command '$cmd'" ;;
    esac
    then debug "command '$cmd' successful"
    else fail  "command '$cmd' failed"
    fi
    done
}

if test $# -eq 0
then usage; fail "no COMMAND specified"
else main "$@"
fi
