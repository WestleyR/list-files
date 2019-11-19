# Created by: WestleyR
# email: westleyr@nym.hush.com
# Date: Nov 18, 2019
# https://github.com/WestleyR/list-files
# Version-1.0.10
#
# The Clear BSD License
#
# Copyright (c) 2019 WestleyR
# All rights reserved.
#
# This software is licensed under a Clear BSD License.
#

CC = gcc
CFLAGS = -Wall
TARGET = lf

PREFIX = /usr/local

MAIN = src/main-lf.c

COMMIT = "$(shell git log -1 --oneline --decorate=short --no-color || echo "ERROR: unable to get commit hash" >&2)"

CFLAGS += -DCOMMIT_HASH=\"$(COMMIT)\"

ifeq ($(DEBUG), true)
	CFLAGS += -DDEBUG
endif

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
$(TARGET): $(MAIN)
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN)
	
.PHONY:
static: $(MAIN)
	$(CC) $(CFLAGS) -D WITHOUT_NAME_GROUP_OUTPUT -static -o $(TARGET) $(MAIN)

.PHONY:
without-ouner-group-names: $(MAIN)
	$(CC) $(CFLAGS) -D WITHOUT_NAME_GROUP_OUTPUT -o $(TARGET) $(MAIN)

.PHONY:
test: $(TARGET)
	@bash ./run-tests

.PHONY:
install: $(TARGET)
	mkdir -p $(PREFIX)/bin
	cp -f $(TARGET) $(PREFIX)/bin

.PHONY:
clean:
	 rm -f $(TARGET)

.PHONY:
uninstall: $(PREFIX)/$(TARGET)
	rm -f $(PREFIX)/$(TARGET)

#
# End Makefile
#
