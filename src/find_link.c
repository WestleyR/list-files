// Created by: WestleyR
// Email: westleyr@nym.hush.com
// Url: https://github.com/WestleyR/list-files
// Last modified date: 2020-04-11
//
// This file is licensed under the terms of
//
// The Clear BSD License
//
// Copyright (c) 2019-2020 WestleyR
// All rights reserved.
//
// This software is licensed under a Clear BSD License.
//

#include "find_link.h"

// find_link will take a file name (const char* name), and find where that
// file is linked to (if any). Returns non-zero if failed. symlink must be
// a large enought buffer to hold the symlink path.
int find_link(char* symlink, const char* name) {
  char link_buff[256];
  link_buff[0] = '\0';

  ssize_t len = readlink(name, link_buff, sizeof(link_buff));
  if (len == -1) {
    perror("readlink");
    fprintf(stderr, "Unable to find link for: %s\n", name);
    return(-1);
  }

  symlink[0] = '\0';
  strcpy(symlink, link_buff);
  symlink[len] = '\0';

  return(0);
}

// vim: tabstop=2 shiftwidth=2 expandtab autoindent softtabstop=0

