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

#include "add_slash.h"

int add_slash(char* path) {
  int len = strlen(path);
  if (path[len-1] != '/') {
    add_char_to_string(path, '/');
  }

  return(0);
}

// vim: tabstop=2 shiftwidth=2 expandtab autoindent softtabstop=0

