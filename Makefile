# Created by: WestleyR
# email: westleyr@nym.hush.com
# Date: Nov 23, 2019
# https://github.com/WestleyR/list-files
# Version-1.0.12
#
# The Clear BSD License
#
# Copyright (c) 2019 WestleyR
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
	@echo " [no-option], all           compile the project"
	@echo " static                     compile the static project"
	@echo " without-owner-group-names  dont print the user/group names,"
	@echo " DEBUG=true                 compile target as debug"
	@echo "                            instead print the uid/gid."
	@echo " install                    install the binary to the PREFIX,"
	@echo "                            (dafault '/usr/local/bin')"
	@echo " clean                      clean the binary"
	@echo " uninstall                  uninstall the binary from PREFIX"
	@echo ""

.PHONY:
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJS)
	
.PHONY:
static: $(OBJS)
	$(CC) $(CFLAGS) -DWITHOUT_NAME_GROUP_OUTPUT -static -o $(TARGET) $(MAIN)

.PHONY:
without-ouner-group-names: $(OBJS)
	$(CC) $(CFLAGS) -DWITHOUT_NAME_GROUP_OUTPUT -o $(TARGET) $(MAIN)

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

.PHONY:
uninstall: $(PREFIX)/$(TARGET)
	rm -f $(PREFIX)/$(TARGET)

#
# End Makefile
#
