# Created by: WestleyR
# email: westleyr@nym.hush.com
# Date: Jul 28, 2019
# https://github.com/WestleyR/list-files
# Version-1.0.3
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

PREFIX = /usr/local/bin

MAIN = src/main-lf.c

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
install: $(TARGET)
	cp -f $(TARGET) $(PREFIX)

.PHONY:
clean:
	 rm -f $(TARGET)

.PHONY:
uninstall: $(PREFIX)/$(TARGET)
	rm -f $(PREFIX)/$(TARGET)

#
# End Makefile
#
