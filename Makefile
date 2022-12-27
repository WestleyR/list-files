# Created by WestleyR <westleyr@nym.hush.com> in 2019
# Source code: https://github.com/WestleyR/list-files
#
# Copyright (c) 2019-2022 WestleyR. All rights reserved.
# This software is licensed under a BSD 3-Clause Clear License.
# Consult the LICENSE file that came with this software regarding
# your rights to distribute this software.
#

CC = gcc
CFLAGS = -Wall -Ideps
TARGET = lf

PREFIX = /usr/local

MODDED = $(shell if command -v git > /dev/null ; then (git diff --exit-code --quiet && echo \"[No changes]\") || echo \"[With uncommited changes]\" ; else echo \"[unknown]\" ; fi)
COMMIT = "$(shell git log -1 --oneline --decorate=short --no-color || ( echo 'ERROR: unable to get commit hash' >&2 ; echo unknown ) )"

CFLAGS += -DCOMMIT_HASH=\"$(COMMIT)\"
CFLAGS += -DUNCOMMITED_CHANGES=\"$(MODDED)\"

ifeq ($(DEBUG), true)
	CFLAGS += -DDEBUG
endif

ifeq ($(STATIC), true)
	CFLAGS += -static -DWITHOUT_NAME_GROUP_OUTPUT 
endif

ifeq ($(WITHOUT_ID), true)
	CFLAGS += -DWITHOUT_NAME_GROUP_OUTPUT 
endif

SRC = $(wildcard src/*.c)
OBJS = $(SRC:.c=.o)

test_files = $(wildcard utests/*.c)
test_files += $(wildcard src/*.c)
TEST_SRC = $(filter-out src/main-lf.c,$(test_files))
TEST_OBJS = $(TEST_SRC:.c=.o)

.PHONY:
all: $(TARGET)

.PHONY:
options:
	@echo "Make options:"
	@echo ""
	@echo "$$ make [option]"
	@echo ""
	@echo " [no-option],     all compile the project"
	@echo " STATIC=true      compile the static project"
	@echo " WITHOUT_ID=true  dont print the user/group names,"
	@echo " DEBUG=true       compile target as debug"
	@echo "                  instead print the uid/gid."
	@echo " install          install the binary to the PREFIX,"
	@echo "                  (dafault '/usr/local/bin')"
	@echo " clean            clean the binary"
	@echo " uninstall        uninstall the binary from PREFIX"
	@echo ""

.PHONY:
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJS)

.PHONY:
%.o: %.c
	$(CC) $(DEP_FLAG) $(CFLAGS) $(LDFLAGS) -o $@ -c $<

.PHONY:
test: $(TARGET) unit-test
	@echo "Running end-to-end tests..."
	@bash ./run-tests

.PHONY:
unit-test: $(TEST_OBJS)
	@echo "Running unit tests..."
	@echo $(TEST_OBJS)
	$(CC) $(CFLAGS) -o testball $(TEST_OBJS)
	./testball

.PHONY:
install: $(TARGET)
	mkdir -p $(PREFIX)/bin
	cp -f $(TARGET) $(PREFIX)/bin

.PHONY:
clean:
	 rm -f $(OBJS) $(TEST_OBJS) || true

.PHONY:
cleanall: clean
	 rm -f $(TARGET)
	 rm -rf ./pkg

.PHONY:
uninstall: $(PREFIX)/$(TARGET)
	rm -f $(PREFIX)/$(TARGET)

#
# End Makefile
#
