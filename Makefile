SHELL := bash

SRC      := src
INC      := include
BLD      := build

EXCSRC   := src/plist_test.c

SRCFLS   := $(shell find src -type f -size +0c)
SRCFLS   := $(filter-out $(EXCSRC), $(SRCFLS))

OBJS     := $(patsubst $(SRC)/%.c,$(BLD)/%.o, $(SRCFLS))
INCFLS   := $(wildcard $(INC)/*.h)

CC       := gcc
TARGET   := lifesim
BIN      := bin/$(TARGET)
CFLAGS   := -Wall -g -Og
LINKS    := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all: $(BIN)

# test the gtree data structure
test: bin/plist_test $(SRC)/plist.c $(INC)/plist.h $(SRC)/pawn.c $(SRC)/world.c
bin/plist_test: $(SRC)/plist.c $(SRC)/plist_test.c $(SRC)/pawn.c $(SRC)/world.c
	$(CC) $(CFLAGS) $^ $(LINKS) -o $@


# remove debug info and include optimization
release: CFLAGS=-Wall -O3 -DNDEBUG
release: clean
release: $(BIN)

# dump the generated lists of files
report:
	@echo SRC: $(SRCFLS)
	@echo OBJS: $(OBJS)
	@echo INC: $(INCFLS)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LINKS) -o $@

$(BLD)/%.o: $(SRC)/%.c
	$(CC) -c $(CFLAGS) $< -o $@


clean:
	rm -f $(BLD)/*.o $(BIN)
