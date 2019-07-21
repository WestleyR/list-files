# Created by: WestleyR
# email: westleyr@nym.hush.com
# Date: Jul 20, 2019
# https://github.com/WestleyR/list-files
# Version-1.0.1
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
$(TARGET): $(MAIN)
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN)

.PHONY:
static: $(MAIN)
	$(CC) $(CFLAGS) -D STATIC_BUILD -static -o $(TARGET) $(MAIN)

.PHONY:
without-ouner-group-names: $(MAIN)
	$(CC) $(CFLAGS) -D STATIC_BUILD -o $(TARGET) $(MAIN)

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
