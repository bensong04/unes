# UNES: basic NES emulator

Currently, the main CPU functionality has mostly been implemented, but is still slightly buggy. Known issues are:

- Hardware quirks are completely unimplemented
- The emulator is not 100% cycle-accurate---i.e. it will execute all of an instruction's logic at once, rather than cycle by cycle.

Each NES CPU instruction has been stress-tested against 2500 [randomly generated test cases](https://github.com/TomHarte/ProcessorTests/tree/main/nes6502) and "verified" to work properly (althrough there is always a small chance of a buggy instruction getting extremely lucky and passing all 2500 test cases).
