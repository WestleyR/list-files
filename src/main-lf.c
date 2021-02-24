//
//  main-lf.c
//  lf - list files and directories
//
// Created by WestleyR on 2019-02-22
// Source code: https://github.com/WestleyR/lf
//
// Copyright (c) 2019-2021 WestleyR. All rights reserved.
// This software is licensed under a BSD 3-Clause Clear License.
// Consult the LICENSE file that came with this software regarding
// your rights to distribute this software.
//

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <grp.h>
#include <getopt.h>
#include <sys/stat.h>
#include <time.h>

#ifndef WITHOUT_NAME_GROUP_OUTPUT
#include <pwd.h>
#endif

#include "files.h"
#include "bool.h"

//#include "color.h"
//#include "iszip.h"
//#include "readable-fs.h"
//#include "extcmp.h"
//#include "find_link.h"
//#include "add_slash.h"
//#include "c-utils/c-utils.h"

#ifndef COMMIT_HASH
#define COMMIT_HASH "unknown"
#endif

#ifndef UNCOMMITED_CHANGES
#define UNCOMMITED_CHANGES "[unknown]"
#endif

#define SCRIPT_VERSION "1.7.0"

char *base_path;

#ifndef WITHOUT_NAME_GROUP_OUTPUT
// For auto ajusting formatting
int max_own_len = 0;
int max_grup_len = 0;
#endif

int max_size = 0;

// machine readable output
int mr_list = 0;
int auto_mr_list = 0;

// reletave path
int rel_path = 0;

// no color option
int no_color_print = 0;

// abs path
int abs_path = 0;

void help_menu(const char* script_name) {
  printf("Copyright (c) 2019-2021 WestleyR, All rights reserved.\n");
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
  printf("%s\n", SCRIPT_VERSION);
  return;
}

void version_commit() {
  printf("%s %s\n", UNCOMMITED_CHANGES, COMMIT_HASH);
  return;
}

/*

char* get_filedate(struct stat finfo) {
#ifdef DEBUG
  printf("DEBUG: functioncall: %s()\n", __func__);
#endif

 char* ret;
  ret = (char*) malloc(20 * sizeof(char));
  if (ret == NULL) {
    perror("malloc");
    return(NULL);
  }

  struct tm * p = localtime(&finfo.st_mtime);
  
  // For year-month-day
  strftime(ret, 1000, "%Y-%m-%d", p);

  // For day-month-year
  //strftime(ret, 1000, "%d-%m-%Y", p);

  return(ret);
}

int file_info(const char* file_path) {
#ifdef DEBUG
  printf("DEBUG: functioncall: %s()\n", __func__);
#endif

 struct stat sb;
  struct stat info;

  char *full_file_path;
  full_file_path = (char*) malloc(256 * sizeof(char));

  char *print_name;
  print_name = (char*) malloc(256 * sizeof(char));

  strcpy(full_file_path, base_path);
  strcat(full_file_path, file_path);

  if (rel_path != 0) {
    print_name = strdup(full_file_path);
  } else {
    print_name = strdup(file_path);
  }

  if (lstat(full_file_path, &info) != 0) {
    perror("lstat");
    printf("error: unable to open stat on: %s\n", full_file_path);
    exit(20);
  }

  // TODO: use 'l' for links
  printf((S_ISDIR(info.st_mode)) ? "d" : "-");
  printf((info.st_mode & S_IRUSR) ? "r" : "-");
  printf((info.st_mode & S_IWUSR) ? "w" : "-");
  printf((info.st_mode & S_IXUSR) ? "x" : "-");
  printf((info.st_mode & S_IRGRP) ? " r" : " -");
  printf((info.st_mode & S_IWGRP) ? "w" : "-");
  printf((info.st_mode & S_IXGRP) ? "x" : "-");
  printf((info.st_mode & S_IROTH) ? "r" : "-");
  printf((info.st_mode & S_IWOTH) ? "w" : "-");
  printf((info.st_mode & S_IXOTH) ? "x" : "-");

#ifdef WITHOUT_NAME_GROUP_OUTPUT
  printf("  %4d", info.st_uid);
  printf("  %4d ", info.st_gid);
#else
  struct passwd *pw = getpwuid(info.st_uid);
  if (pw == NULL) {
    printf("  %*d", max_own_len, info.st_uid);
  } else {
    printf("  %*s", max_own_len, pw->pw_name);
  }

  struct group *gr = getgrgid(info.st_gid);
  if (gr == NULL) {
    printf("  %-*d ", max_grup_len, info.st_gid);
  } else {
    printf("  %-*s ", max_grup_len, gr->gr_name);
  }
#endif

  char* file_date = get_filedate(info);
  if (file_date == NULL) {
    printf(" %s ", "0000-00-00");
  } else {
    printf(" %s ", file_date);
    free(file_date);
  }

  char* file_bytes = readable_fs(info.st_size);
  if (file_bytes != NULL) {
    int file_bytes_len = strlen(file_bytes);
    if (file_bytes_len > max_size) max_size = file_bytes_len;
    printf(" %-*s", max_size, file_bytes);
    free(file_bytes);
  } else {
    printf(" %-*s", 8, "unknown");
  }

  if (no_color_print == 0) {
    if (S_ISLNK(info.st_mode)) {
      char *link_path;
      link_path = (char*) malloc(256 * sizeof(char));

      int err = find_link(link_path, full_file_path);
      if (err != 0) {
        strcpy(link_path, "failed to get symlink");
      }
      char full_link_path[strlen(base_path)+strlen(link_path)+1];
      full_link_path[0] = '\0';

      if (link_path[0] == '/') {
        strcpy(full_link_path, link_path);
      } else {
        strcpy(full_link_path, base_path);
        strcat(full_link_path, link_path);
      }

      printf("FULL_LINK_PATH: %s\n", full_link_path);
      printf("BASEPATH: %s\n", base_path);

      if (access(full_link_path, F_OK) != 0) {
        // If the link is broken
        printf("  %s%s%s  ->  %s%s%s\n", BBOLDRED, print_name, COLORRESET, BBOLDRED, link_path, COLORRESET);
      } else {
        printf("  %s%s%s  ->  %s\n", BOLDCYAN, print_name, COLORRESET, link_path);
      }
      free(link_path);
    } else if (S_ISDIR(info.st_mode)) {
      printf("  %s%s%s\n", BOLDBLUE, print_name, COLORRESET);
    } else if (stat(full_file_path, &sb) == 0 && sb.st_mode & S_IXUSR) {
      printf("  %s%s%s\n", BOLDGREEN, print_name, COLORRESET);
    } else if (iszip(full_file_path) == 0) {
      printf("  %s%s%s\n", BOLDRED, print_name, COLORRESET);
    } else if (access(full_file_path, R_OK) != 0) {
      printf("  %s%s%s\n", BOLDMAGENTA, print_name, COLORRESET);
    } else {
      printf("  %s\n", print_name);
    }
  } else {
    if (S_ISLNK(info.st_mode)) {
      char *link_path;
      link_path = (char*) malloc(256 * sizeof(char));

      int err = find_link(link_path, full_file_path);
      if (err != 0) {
        strcpy(link_path, "failed to get symlink");
      }
      printf("  %s  ->  %s\n", print_name, link_path);
      free(link_path);
    } else {
      printf("  %s\n", print_name);
    }
  }

  free(full_file_path);
  free(print_name);

  return(0);
}

// Will list files in a directory.
int list_files(const char* list_path, int list_all) {
#ifdef DEBUG
  printf("DEBUG: functioncall: %s()\n", __func__);
#endif

 DIR *dr;
  struct dirent *de;

  dr = opendir(list_path);
  while ((de = readdir(dr)) != NULL) {
    if (list_all == 0) {
      if ((*de->d_name == '.') || (strcmp(de->d_name, "..") == 0)) {
        continue;
      }
    }

    if (mr_list != 0) {
      if (rel_path != 0) {
        char r_path[256];
        r_path[0] = '\0';
        strcpy(r_path, list_path);
        strcat(r_path, de->d_name);
        printf("%s\n", r_path);
      } else {
        printf("%s\n", de->d_name);
      }
    } else {
      if (file_info(de->d_name) != 0) {
        printf("ERROR: while listing file: %s\n", de->d_name);
      }
    }
  }
  closedir(dr);

  return(0);
}

// max_len_files will loop files in a directory, and get the max lenth for all the permamiters.
int max_len_files(const char* list_path, int list_all) {
#ifdef DEBUG
  printf("DEBUG: functioncall: %s()\n", __func__);
#endif

  DIR *dr;
  struct dirent *de;

  char *full_file_path;
  full_file_path = (char*) malloc(256 * sizeof(char));
  if (full_file_path == NULL) {
    perror("malloc");
    return(1);
  }

  full_file_path[0] = '\0';

#ifndef WITHOUT_NAME_GROUP_OUTPUT
  struct max_list {
    int uid_num;
    int max_uid;
    int pid_num;
    int max_pid;
    int end;
  };

  struct max_list ml[10];

  ml[0].uid_num = -1;
  ml[0].max_uid = -1;
  ml[0].pid_num = -1;
  ml[0].max_pid = -1;
  ml[0].end = 1;

  int mindex = 0;
  int match = 0;
#endif

#ifdef DEBUG
  printf("%s(): Opening: %s\n", __func__, list_path);
#endif
  dr = opendir(list_path);
  if (dr == NULL) {
    fprintf(stderr, "%s(): failed to open: %s\n", __func__, list_path);
    return -1;
  }

  while ((de = readdir(dr)) != NULL) {
    if (list_all == 0) {
      if ((*de->d_name == '.') || (strcmp(de->d_name, "..") == 0)) {
        continue;
      }
    }

    strcat(full_file_path, base_path);
    strcat(full_file_path, de->d_name);

    struct stat info;
    if (lstat(full_file_path, &info) != 0) {
      perror("lstat");
      printf("error: unable to open stat on: %s\n", full_file_path);
      exit(20);
    }

    char* file_bytes = readable_fs(info.st_size);
    if (file_bytes != NULL) {
      int file_bytes_len = strlen(file_bytes);
      if (file_bytes_len > max_size) max_size = file_bytes_len;
      free(file_bytes);
    }

#ifndef WITHOUT_NAME_GROUP_OUTPUT
    if (mindex >= 9) {
      struct passwd *pw;
      struct group *gr;

      pw = getpwuid(info.st_uid);
      gr = getgrgid(info.st_gid);

      int own = strlen(pw->pw_name);
      if (own > max_own_len) {
        max_own_len = own;
      }

      int grup = strlen(gr->gr_name);
      if (grup > max_grup_len) {
        max_grup_len = grup;
      }
    } else {
      for (int l = 0; l < mindex; l++) {
        if ((info.st_uid == ml[l].uid_num) && (info.st_gid == ml[l].pid_num)) {
          match = 1;
          break;
        }
      }

      if (match != 1) {
#ifdef DEBUG
        printf("Caching: %s : %s\n", getpwuid(info.st_uid)->pw_name, getgrgid(info.st_gid)->gr_name);
#endif

        struct passwd *pw;
        struct group *gr;

        pw = getpwuid(info.st_uid);
        if (pw != NULL) {
          ml[mindex].uid_num = (int)info.st_uid;
          ml[mindex].max_uid = (int)strlen(pw->pw_name);
        } else {
          ml[mindex].uid_num = (int)info.st_uid;
          ml[mindex].max_uid = 4;
        }

        gr = getgrgid(info.st_gid);
        if (pw != NULL) {
          ml[mindex].pid_num = (int)info.st_gid;
          ml[mindex].max_pid = (int)strlen(gr->gr_name);
        } else {
          ml[mindex].uid_num = (int)info.st_uid;
          ml[mindex].max_uid = 4;
        }

        ml[mindex].end = 1;
        mindex++;
      }

      match = 0;

      if (mindex >= 10) {
        for (int i = 0; i < mindex; i++) {
          if (ml[i].end == 0) break;
          if (ml[i].max_uid > max_own_len) max_own_len = ml[i].max_uid;
          if (ml[i].max_pid > max_grup_len) max_grup_len = ml[i].max_pid;
          if (i > 10) break;
        }
      }
    }
#endif

    full_file_path[0] = '\0';
  }
  closedir(dr);


#ifndef WITHOUT_NAME_GROUP_OUTPUT
  ml[mindex].uid_num = 0;
  ml[mindex].max_uid = 0;
  ml[mindex].pid_num = 0;
  ml[mindex].max_pid = 0;
  ml[mindex].end = 0;

  if (mindex < 10) {
    int i = 0;
    while (1) {
      if (ml[i].end == 0) break;
      if (ml[i].max_uid > max_own_len) max_own_len = ml[i].max_uid;
      if (ml[i].max_pid > max_grup_len) max_grup_len = ml[i].max_pid;
      i++;
      if (i > 10) break;
    }
  }

  ml[0].uid_num = 0;
  ml[0].max_uid = 0;
  ml[0].pid_num = 0;
  ml[0].max_pid = 0;
  ml[0].end = 0;

  if ((max_own_len > 20) || (max_grup_len > 20)) {
    printf("INTERNAL ERROR; recovering\n");
    max_own_len = 8;
    max_grup_len = 8;
  }
#endif

  free(full_file_path);

  return(0);
}

int prep_list(const char* script_name, const char *file_path, int list_all) {
#ifdef DEBUG
  printf("DEBUG: functioncall: %s()\n", __func__);
#endif

  struct stat s;
  char* path = strdup(file_path);

  if (lstat(path, &s) != 0) {
    fprintf(stderr, "%s: %s: No such file or directory\n", script_name, path);
    exit(2);
  }

  if (S_ISDIR(s.st_mode) == 0) {
    // Path argument is a file
    if (mr_list != 0) {
      printf("%s\n", path);
      return(0);
    }
    if (file_info(file_path) != 0) {
      printf("error while listing file\n");
    }
    return(0);
  }
  // If got to here, its a directory

  add_slash(path);
  base_path = strdup(path);

#ifdef DEBUG
  printf("%s(): opening filepath: %s\n", __func__, file_path);
#endif

  if (access(file_path, F_OK) != -1) {
    if (access(file_path, R_OK) == -1) {
      fprintf(stderr, "%s: Not readable\n", path);
      exit(1);
    }
  } else {
    fprintf(stderr, "PANIC: cannot access %s: No such file or directory\n", path);
    exit(1);
  }

  if (mr_list != 1) {
    max_len_files(path, list_all);
  }

  if (list_files(path, list_all) != 0) {
    printf("There was a problome listing files\n");
    exit(1);
  }

  return(0);
}

*/

//*****
// Main
//*****
int main(int argc, char** argv) {
#ifdef DEBUG
  printf("DEBUG: functioncall: %s()\n", __func__);
#endif

  // -a option
  int list_all = 0;

  // If piped or redirected, dont use color unless
  // overidded.
  if (isatty(STDOUT_FILENO) == 0) {
    no_color_print = 1;
  }

  int opt = 0;

  char* color_print;
  color_print = (char*) malloc(5 * sizeof(char));

  // Alloc the base_path TODO: this should be passed via function arguments
  base_path = (char*) malloc(256 * sizeof(char));
  if (base_path == NULL) {
    fprintf(stderr, "%s(): Error: malloc failed for size of: %lu\n", __func__, (unsigned long)256 * sizeof(char));
    return 1;
  }

  static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"rel-path", no_argument, 0, 'p'},
    {"mr", no_argument, 0, '1'},
    {"mr", no_argument, 0, 'm'},
    {"hr", no_argument, 0, 'r'},
    {"all", no_argument, 0, 'a'},
    {"color", required_argument, 0, 'c'},
    {"version", no_argument, 0, 'V'},
    {"commit", no_argument, 0, 'C'},
    {NULL, 0, 0, 0}
  };

  while ((opt = getopt_long(argc, argv,"c:1mrabpVhC", long_options, 0)) != -1) {
    switch (opt) {
      case '1':
        mr_list = 1;
        break;
      case 'm':
        mr_list = 1;
        break;
      case 'a':
        list_all = 1;
        break;
      case 'p':
        rel_path = 1;
        break;
      case 'r':
        // If human-readable output, then disable mr output
        mr_list = 0;
        break;
      case 'c':
        strcpy(color_print, optarg);
        break;
      case 'h':
        help_menu(argv[0]);
        return(0);
        break;
      case 'V':
        version_print();
        return(0);
        break;
      case 'C':
        version_commit();
        return(0);
        break;
      default:
        // Invalid option
        return(22);
    }
  }

  // Check if LF_MR_AUTO_OUTPUT is set, and see what its set to
  char* auto_mr_output = getenv("LF_MR_AUTO_OUTPUT");
  if (auto_mr_output != NULL) {
    if (auto_mr_output[0] == '0') {
      if (no_color_print == 1) {
        mr_list = 1;
      }
    } else if (auto_mr_output[0] == '1') {
      // default
    } else {
      fprintf(stderr, "%s: Invalid option for LF_MR_AUTO_OUTPUT: %s\n", argv[0], auto_mr_output);
    }
  } else {
    if (no_color_print == 1) {
      mr_list = 1;
    }
  }

  // If the --color option is set
  if (color_print[0] != '\0') {
    if (strcmp(color_print, "auto") == 0) {
      // Do nothing
    } else if (strcmp(color_print, "on") == 0) {
      no_color_print = 0;
    } else if (strcmp(color_print, "off") == 0) {
      no_color_print = 1;
    } else {
      fprintf(stderr, "%s: --color: %s: invalid argument\n", argv[0], color_print);
      return(22);
    }
  }
  free(color_print);

  lf_files* ctx = lf_new();

  if (optind < argc) {
    for (int i = optind; i < argc; i++) {
//      prep_list(argv[0], argv[i], list_all);
      lf_add_path(ctx, argv[i]);
    }
    for (int i = optind; i < argc; i++) {
      lf_get_max_size_from_path(ctx);
      // print the output
      //lf_print(ctx);
    }
 
  } else {
      lf_add_path(ctx, "./");
      lf_get_max_size_from_path(ctx);
      lf_print(ctx);
  }

  lf_destroy(ctx);


  return(0);
}

// vim: tabstop=2 shiftwidth=2 expandtab autoindent softtabstop=0
