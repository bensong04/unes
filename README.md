# UNES: basic NES emulator

Currently, the main CPU functionality has theoretically been implemented, but is still quite buggy. Known issues are:
* BRK is incorrect against Tom Harte's unit tests
* PLP pulls B straight from the stack, but it may need to set B unconditionally.
* Hardware quirks are completely unimplemented
* The emulator is not 100% cycle-accurate---i.e. it will execute all of an instruction's logic at once, rather than cycle by cycle.
