Speaker with Paired LED and On-Off Switch
=========================================

Setup
-----
```
GND ----------------------- (-) Buzzer (+) --- Pin 12
GND -- 100 Ohm (voltdiv) -- (-) LED    (+) --- Pin 4
5V  ----------------------- (-) Button (+) --- Pin 3 (Interrupt Pin)
                                           `-- 1000 Ohm -- GND (pulldown)
```

Function
--------
Press Button to start playing predefined songs.
Press Button to stop playing anytime.

Learnings
---------
* Needed to unloop the original playback loop, since it would busy loop the
system and prevent interrupts. Basically refactored a dumb `playAll` function
to a state machine that can be be controlled with a `playNextNote` function.
* Did some basic debouncing of button interrupt (not perfect though).
* Learned about race conditions with interrupts.
* moved play control to a C++ class.
