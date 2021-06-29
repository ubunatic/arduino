# Arduino Experiments and Libraries

This monorepo contains some Arduino experiments and personal libraries used in
my projects. The most notable projects are the [SignalState Library](#SignalState)
and the [Board Script](#Board-Script).

## SignalState
[SignalState](signalstate) helps you manage input signals such as IR commands or key-presses.
It provides a simple, non-blocking, configurable API for distinguishing single
and repeated signals, commands, and key-presses.

## Board Script
This repo also hosts the Arduino "script aggregator" [board.sh](board.sh).
Anytime I stumble over a hard-to-remember or too complex command of the
`arduino-cli` or the `pio` CLI, I try to cover the most common variant of the
command as a subcommand of [board.sh](board.sh).

### Usages

See `board help`. The most common usages for me are the following.

* avoid having build scripts in the sub-projects
* quickly compile the current project using `board pio compile`
* compile, upload, and start the serial console in one step using `board pio`
* look up how `pio` and `arduino-cli` work
* check my devices `board show` (prints JSON output)
* start a serial using `board serial`

### Installation
Link or copy it to your `PATH`.

## Arduino Experiments

* [01-blink](01-blink): Some blinking LEDs
* [02-speaker](02-speaker): Play some music with a piezo speaker!
* [04-tinygo](04-tinygo): Failed experiment to get started with Tinygo.
* [05-smooth-stepper](03-smooth-stepper): IR-controlled 28BYJ-48 stepper.

  Features:
  * takes single steps on single key press
  * smooth continuous, non-blocking stepping when holding keys
  * increase decrease stepper speed
  * turn of power when idle (avoids heating the stepper)
  * basic logging and debugging primitives

## License
[MIT](LICENSE)
