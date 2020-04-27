# Created by: WestleyR
# Email: westleyr@nym.hush.com
# Url: https://github.com/WestleyR/list-files
# Last modified date: 2020-04-26
#
# This file is licensed under the terms of
#
# The Clear BSD License
#
# Copyright (c) 2019-2020 WestleyR
# All rights reserved.
#
# This software is licensed under a Clear BSD License.
#

CC = gcc
CFLAGS = -Wall -Ideps
TARGET = lf

PREFIX = /usr/local

COMMIT = "$(shell git log -1 --oneline --decorate=short --no-color || ( echo 'ERROR: unable to get commit hash' >&2 ; echo unknown ) )"

CFLAGS += -DCOMMIT_HASH=\"$(COMMIT)\"

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
SRC += $(wildcard deps/*/*.c)

OBJS = $(SRC:.c=.o)

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
test: $(TARGET)
	@bash ./run-tests

.PHONY:
install: $(TARGET)
	mkdir -p $(PREFIX)/bin
	cp -f $(TARGET) $(PREFIX)/bin

.PHONY:
clean:
	 rm -f $(OBJS)

.PHONY:
cleanall:
	 rm -f $(TARGET) $(OBJS)
	 rm -rf ./pkg

.PHONY:
uninstall: $(PREFIX)/$(TARGET)
	rm -f $(PREFIX)/$(TARGET)

#
# End Makefile
#
