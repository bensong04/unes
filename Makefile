# Source file wildcards
CORE_CPU = $(wildcard core/cpu/*.c)
CORE_MEMORY = $(wildcard core/memory/*.c) 
SRC_CORE = $(CORE_CPU) $(CORE_MEMORY) 

# Driver paths
DRIVERS = eval/drivers
CPU_DRIVER = $(DRIVERS)/cpu_driver.c
CPU_UNIT_DRIVER = $(DRIVERS)/cpu_driver_stepwise.c

# Include paths
INCLUDE = core

# Compiler flags
OPTIONS = -fcommon -I$(INCLUDE)
COMPILE_CMD = gcc $(OPTIONS)
DBG_COMPILE_CMD = gcc -g -DDEBUG -DCPU_TESTS $(OPTIONS)

# Where outputted binaries go
BIN = bin

.PHONY: clean

cpu_unittest:
	$(DBG_COMPILE_CMD) $(SRC_CORE) $(CPU_UNIT_DRIVER) -o $(BIN)/cpu_unittest

cpu_driver_dbg:
	$(DBG_COMPILE_CMD) $(SRC_CORE) $(CPU_DRIVER) -o $(BIN)/cpu_driver

cpu_driver:
	$(COMPILE_CMD) $(SRC_CORE) $(CPU_DRIVER) -o $(BIN)/cpu_driver

clean:
	rm -rf bin/*