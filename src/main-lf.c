//
//  main-lf.c
//  lf - list files and directories
//
// Created by WestleyR <westleyr@nym.hush.com> on 2019-02-22
// Source code: https://github.com/WestleyR/lf
//
// Copyright (c) 2019-2022 WestleyR. All rights reserved.
// This software is licensed under a BSD 3-Clause Clear License.
// Consult the LICENSE file that came with this software regarding
// your rights to distribute this software.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "files.h"
#include "bool.h"

#ifndef COMMIT_HASH
#define COMMIT_HASH "unknown"
#endif

#ifndef UNCOMMITED_CHANGES
#define UNCOMMITED_CHANGES "[unknown]"
#endif

#define LF_CLI_VERSION "v1.7.0.b2"

void help_menu(const char* script_name) {
  printf("Copyright (c) 2019-2022 WestleyR, All rights reserved.\n");
  printf("This software is licensed under the terms of\n");
  printf("The Clear BSD License.\n");
  printf("Source code: https://github.com/WestleyR/list-files\n");
  printf("\n");
  printf("Description:\n");
  printf("  List files in a directory and print it to stdout.\n");
  printf("\n");
  printf("Usage:\n");
  printf("  %s [option] <path>...\n", script_name);
  printf("\n");
  printf("Options:\n");
  printf("  -a, --all          list all files\n");
  printf("  -p, --rel-path     list files with relative path\n");
  printf("  -1, -m, --mr       only print file names (mr), color will be off\n");
  printf("  -r, --hr           always output human-readable (overides mr)\n");
  printf("  -c, --color=       color output, options: on, off, auto\n");
  printf("  -o, --octal        print the permissions in octal (WIP)\n");
  printf("  -h, --help         print help menu\n");
  printf("  -C, --commit       print the github commit\n");
  printf("  -V, --version      print version\n");
  printf("\n");
  printf("Env vars:\n");
  printf("  LF_MR_AUTO_OUTPUT=[0,1]  enables the auto machine-readable\n");
  printf("                           output. (default: 0)\n");
  printf("                           0,* = enable auto mr output.\n");
  printf("                           1 = disable auto mr output (use human-readable output\n");
  printf("                           unless otherwise specifyed).\n");
  printf("\n");
  printf("Permisions:\n");
  printf("  - = file, if its the first option,\n");
  printf("      otherwise, no permission\n");
  printf("  d = directory\n");
  printf("  r = readable\n");
  printf("  w = writable\n");
  printf("  x = executable\n");
  return;
}

void version_print() {
  printf("%s\n", LF_CLI_VERSION);
  return;
}

void version_commit() {
  printf("%s %s\n", UNCOMMITED_CHANGES, COMMIT_HASH);
  return;
}

//*****
// Main
//*****
int main(int argc, char** argv) {
  // -a option
  bool list_all = false;

  // machine readable output
  bool mr_list = false;

  // -p option
  bool rel_path = false;

  // -t option
  bool sort_by_date = false;

  // no color option
  bool color_print = true;

  // -o,--octal flag
  bool octal_perm = false;

  // If piped or redirected, dont use color unless
  // overidded.
  if (isatty(STDOUT_FILENO) == 0) {
    color_print = false;
  }

  // Check if LF_MR_AUTO_OUTPUT is set, and see what its set to
  char* auto_mr_output = getenv("LF_MR_AUTO_OUTPUT");
  if (auto_mr_output != NULL) {
    if (auto_mr_output[0] == '0') {
      if (color_print == false) {
        mr_list = true;
      }
    } else if (auto_mr_output[0] == '1') {
      // default
    } else {
      fprintf(stderr, "%s: Invalid option for LF_MR_AUTO_OUTPUT: %s\n", argv[0], auto_mr_output);
    }
  } else {
    if (color_print == false) {
      mr_list = true;
    }
  }

  int opt = 0;
  char* color_opt = (char*) malloc(5 * sizeof(char));

  static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"rel-path", no_argument, 0, 'p'},
    {"time", no_argument, 0, 't'},
    {"mr", no_argument, 0, '1'},
    {"mr", no_argument, 0, 'm'},
    {"hr", no_argument, 0, 'r'},
    {"all", no_argument, 0, 'a'},
    {"octal", no_argument, 0, 'o'},
    {"color", required_argument, 0, 'c'},
    {"version", no_argument, 0, 'V'},
    {"commit", no_argument, 0, 'C'},
    {NULL, 0, 0, 0}
  };

  while ((opt = getopt_long(argc, argv,"c:1mrabptoVhC", long_options, 0)) != -1) {
    switch (opt) {
    case '1':
      mr_list = true;
      break;
    case 'm':
      mr_list = true;
      break;
    case 'a':
      list_all = true;
      break;
    case 'p':
      rel_path = true;
      break;
    case 'r':
      // If human-readable output, then disable mr output
      mr_list = false;
      break;
    case 't':
      sort_by_date = true;
      break;
    case 'c':
      strcpy(color_opt, optarg);
      break;
    case 'o':
      octal_perm = true;
      break;
    case 'h':
      help_menu(argv[0]);
      return 0;
      break;
    case 'V':
      version_print();
      return 0;
      break;
    case 'C':
      version_commit();
      return 0;
      break;
    default:
      // Invalid option
      return 22;
    }
  }

  // If the --color option is set
  if (color_opt[0] != '\0') {
    if (strcmp(color_opt, "auto") == 0) {
      // Do nothing
    } else if (strcmp(color_opt, "on") == 0) {
      color_print = true;
    } else if (strcmp(color_opt, "off") == 0) {
      color_print = false;
    } else {
      fprintf(stderr, "%s: --color: %s: invalid argument\n", argv[0], color_opt);
      return 22;
    }
  }
  free(color_opt);

  lf_files* ctx = lf_new();

  if (optind < argc) {
    for (int i = optind; i < argc; i++) {
      lf_add_path(ctx, argv[i]);
    }
  } else {
    lf_add_path(ctx, "./");
  }

  // Set the output options
  lf_set_print_all(ctx, list_all);
  lf_set_print_rel_path(ctx, rel_path);
  lf_set_print_mr_output(ctx, mr_list);
  lf_set_print_color(ctx, color_print);
  lf_set_sort_by_date(ctx, sort_by_date);
  lf_set_octal_perm(ctx, octal_perm);

  // Get the currect formatting spaces before printing the output
  lf_get_max_size_from_path(ctx);

  // Print the files
  int rc = lf_print(ctx);

  // Free the context
  lf_destroy(ctx);

  return rc;
}

// vim: tabstop=2 shiftwidth=2 expandtab autoindent softtabstop=0
