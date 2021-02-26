//
//  files.h
//  lf - list files and directories (internal helper functions)
//
// Created by WestleyR 2021-02-24
// Source code: https://github.com/WestleyR/list-files
//
// Copyright (c) 2021 WestleyR. All rights reserved.
// This software is licensed under a BSD 3-Clause Clear License.
// Consult the LICENSE file that came with this software regarding
// your rights to distribute this software.
//


#ifndef WST_FILE__H
#define WST_FILE__H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <uuid/uuid.h>
#include <time.h>
#include <libgen.h>

#include "bool.h"
#include "catpath.h"
#include "color.h"

typedef struct lf_files lf_files;

// Creations/frees
lf_files* lf_new();
int lf_destroy(lf_files* ctx);

// Add paths
int lf_add_path(lf_files* ctx, const char* path);

// Set the options
int lf_set_print_all(lf_files* ctx, bool print_all);
int lf_set_print_rel_path(lf_files* ctx, bool rel_path);
int lf_set_print_mr_output(lf_files* ctx, bool mr_output);
int lf_set_print_color(lf_files* ctx, bool print_color);

// Printing functions
int lf_get_max_size_from_path(lf_files* ctx);
int lf_print(lf_files* ctx);


// etc funcs
char* human_readable_bytes(double bytes);

#endif // WST_FILE__H 

