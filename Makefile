# OS-specific frameworks
MAC = -framework Cocoa -framework OpenGL -framework IOKit -framework GLUT
OS = $(MAC)

# External libraries
GLFW = lib/glfw/

# Source file wildcards
CORE_CPU = $(wildcard core/cpu/*.c)
CORE_MEMORY = $(wildcard core/memory/*.c)
CORE_GFX = $(wildcard core/graphics/*.c)
SRC_CORE = $(CORE_CPU) $(CORE_MEMORY) $(CORE_GFX)

# Driver paths
DRIVERS = eval/drivers
CPU_DRIVER = $(DRIVERS)/cpu_driver.c
CPU_UNIT_DRIVER = $(DRIVERS)/cpu_driver_stepwise.c

# Misc. test paths
GRAPHICS_TEST = eval/graphicstests/pxdisplay.cpp $(CORE_GFX)

# Include paths
INCLUDE_DIRS = core include
INCLUDE = $(foreach d, $(INCLUDE_DIRS), -I$d)

# Compiler flags
OPTIONS = -fcommon $(INCLUDE) $(OS)
COMPILE_CMD = clang $(OPTIONS)
COMPILE_CPP = clang++ $(OPTIONS)
DBG_COMPILE_CMD = clang -g -DDEBUG -DCPU_TESTS $(OPTIONS)
SPEEDY_COMPILE_CMD = clang -O3 -DCPU_TESTS $(OPTIONS)

# Where outputted binaries go
BIN = bin

# Formatting preferences
STYLE = GOOGLE
FORMAT_EXTS = c cpp h
FORMAT_DIR = core

# Utility functions
define allbutlast
$(wordlist 2,$(words $(1)),x $(1))
endef

FORMAT_ARGS = $(call allbutlast, $(foreach ext,$(FORMAT_EXTS), -iname "*.$(ext)" -o))

.PHONY: clean format

format:
	find $(FORMAT_DIR) $(FORMAT_ARGS) | xargs clang-format -i -style=$(STYLE)

display_px_test: bin/display_px_test
	$(COMPILE_CPP) $(GRAPHICS_TEST) -L$(GLFW) -lglfw3 -o bin/display_px_test

speedtest: bin/speedtest
	$(SPEEDY_COMPILE_CMD) $(SRC_CORE) $(CPU_DRIVER) -o bin/speedtest

cpu_unittest: bin/cpu_unittest
	$(DBG_COMPILE_CMD) $(SRC_CORE) $(CPU_UNIT_DRIVER) -o $(BIN)/cpu_unittest

cpu_driver_dbg: bin/cpu_driver
	$(DBG_COMPILE_CMD) $(SRC_CORE) $(CPU_DRIVER) -o $(BIN)/cpu_driver

cpu_driver: bin/cpu_driver
	$(COMPILE_CMD) $(SRC_CORE) $(CPU_DRIVER) -o $(BIN)/cpu_driver

clean:
	rm -rf bin/*
