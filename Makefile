SHELL := bash

SRC      := src
INC      := include
BLD      := build

SRCFLS   := $(shell find $(SRC) -type f -size +0c)

OBJS     := $(patsubst $(SRC)/%.c,$(BLD)/%.o, $(SRCFLS))
INCFLS   := $(wildcard $(INC)/*.h)

CC       := gcc
TARGET   := lifesim
BIN      := bin/$(TARGET)

CFLAGS   := -Wall -Wextra -Werror -ggdb -Og
CFLAGS   += -Wnull-dereference
CFLAGS   += -Wpointer-arith
CFLAGS   += -Wcast-align
CFLAGS   += -Wwrite-strings
CFLAGS   += -Wswitch-default
CFLAGS   += -Wunreachable-code
CFLAGS   += -Winit-self
CFLAGS   += -Wmissing-field-initializers
CFLAGS   += -Wstrict-prototypes
CFLAGS   += -Wundef
CFLAGS   += -Wduplicated-cond
CFLAGS   += -Wfloat-equal
CFLAGS   += -Wshadow

LINKS    := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

.PHONY: clean
.PHONY: all

all: $(BIN)

# remove debug info and include optimization
.PHONY: release
release: clean
release: CFLAGS=-Wall -O3 -DNDEBUG
release: $(BIN)

# build with profile flag
.PHONY: profile
profile: clean
profile: CFLAGS += -pg
profile: PFL = 1
profile: $(BIN)

# dump the generated lists of files
.PHONY: report
report:
	@echo SRC: $(SRCFLS)
	@echo OBJS: $(OBJS)
	@echo INC: $(INCFLS)

$(BIN): $(OBJS)
	$(CC) $^ $(LINKS) $(if $(PFL),-pg,) -o $@

$(BLD)/%.o: $(SRC)/%.c
	$(CC) -c $(CFLAGS) $< -o $@



# tests
# https://www.throwtheswitch.org/build/make
# expects libunity.a installed at /usr/local/bin/
#   and unity.h, unityinternals.h installed at /usr/local/include/unity/
# expects test files to be named: Test_<srcname>.c
.PHONY: test
.PHONY: test_clean

#SRCFLS   := $(filter-out $(EXCSRC), $(SRCFLS))


TST      := test/
TSTBLD   := test/build/
TSTRES   := test/results/
TARGET_EXTENSION=out
TSTLINKS = $(LINKS) -lunity

BUILD_PATHS = $(TSTBLD) $(TSTRES)

SRCT = $(wildcard $(TST)*.c)
RESULTS = $(patsubst $(TST)Test%.c,$(TSTRES)Test%.txt, $(SRCT))
TSTOBJS = $(filter-out $(BLD)/main.o, $(OBJS))


PASSED = `grep -s PASS $(TSTRES)*.txt`
FAIL = `grep -s FAIL $(TSTRES)*.txt`
IGNORE = `grep -s IGNORE $(TSTRES)*.txt`

test: $(BUILD_PATHS) $(RESULTS)
	@echo -e "-----------------------\nIGNORES:\n-----------------------"
	@echo -e "$(IGNORE)"
	@echo -e "-----------------------\nPASSED:\n-----------------------"
	@echo -e "$(PASSED)"
	@echo -e "-----------------------\nFAILURES:\n-----------------------"
	@echo -e "$(FAIL)"
	@echo -e "\nDONE"


$(TSTRES)Test_%.txt: $(TSTBLD)Test_%.$(TARGET_EXTENSION)
	-./$< > $@ 2>&1

$(TSTBLD)Test_%.$(TARGET_EXTENSION): $(TSTBLD)Test_%.o $(TSTOBJS)
	$(CC) $^ $(TSTLINKS) -o $@

$(TSTBLD)%.o: $(TST)%.c
	$(CC) -c $(CFLAGS) $< -o $@


$(TSTBLD):
	mkdir -p $(TSTBLD)

$(TSTRES):
	mkdir -p $(TSTRES)

.PRECIOUS: $(TSTBLD)%.o
.PRECIOUS: $(TSTBLD)Test_%.$(TARGET_EXTENSION)
.PRECIOUS: $(TSTRES)Test_%.txt


clean:
	rm -f $(BLD)/*.o bin/*
	rm -f $(TSTBLD)/* $(TSTRES)/*

test_clean:
	rm -f $(TSTBLD)/* $(TSTRES)/*

