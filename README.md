# uNES

uNES is a basic NES emulator, currently still under development. In its current state it supports full, cycle-accurate CPU emulation. 

uNES is written completely in C, and is meant to be fast and performant. The emulator avoids dynamic memory allocation entirely, and is able to run 80,000,000 instructions per second, clocking in at just under 300 MHz.  

## Issues

- Hardware quirks are, for the most part, unimplemented.
  - The quirk w.r.t indirect jumps (documented [here](http://www.6502.org/tutorials/6502opcodes.html#JMP)) has been implemented.
- The emulator is not 100% cycle-accurate---i.e. it will execute all of an instruction's logic at once, rather than cycle by cycle.
- Undocumented/illegal opcodes are, for the most part, unimplemented.
  - Opcode `04` *is* implemented, but it is a 2-byte NOP. Not very interesting.

## Accuracy

Each NES CPU instruction has been stress-tested against 10000 [randomly generated test cases](https://github.com/TomHarte/ProcessorTests/tree/main/nes6502) and "verified" to work properly (althrough there is always a small chance of a buggy instruction getting extremely lucky and passing all 10000 test cases).

## Build instructions

In the future, common mappers will be implemented so that execution of arbitrary `.nes` files is trivial. Currently, `cpu_driver.c` is specifically configured to run blargg's [offical.nes](https://github.com/christopherpow/nes-test-roms/tree/master/blargg_nes_cpu_test5) test ROM. To test the execution of this ROM, first download `offical.nes` and copy it to `eval/execs`. Then run
```
make speedtest
```
(this will issue some warnings, but they are safe to ignore). Finally,
```
bin/speedtest
```
will execute `offical.nes` for 10 seconds and print some useful performance and debug information to stdout:
```
PC: e000 A: 0 X: 0 Y: 0 SP: fd status: 24 remaining cycles: 0 deferred: 0
815052076 instructions executed in 2852708669 clock cycles
PC: 8353 A: 2d X: 0 Y: 0 SP: f4 status: 27 remaining cycles: 1 deferred: 1
```
