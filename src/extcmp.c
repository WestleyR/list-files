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

#include "extcmp.h"

int extcmp(const char *string, const char* ext) {
  string = strrchr(string, '.');

  if (string != NULL) {
    return(strcmp(string, ext));
  }

  return(-1);
}

// vim: tabstop=2 shiftwidth=2 expandtab autoindent softtabstop=0

