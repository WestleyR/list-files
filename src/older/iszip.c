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

#include "iszip.h"

int iszip(const char* file) {
  if (extcmp(file, ".gz") == 0) {
    return(0);
  } else if (extcmp(file, ".lz4") == 0) {
    return(0);
  } else if (extcmp(file, ".tgz") == 0) {
    return(0);
  } else if (extcmp(file, ".zip") == 0) {
    return(0);
  }

  return(-1);
}

// vim: tabstop=2 shiftwidth=2 expandtab autoindent softtabstop=0

