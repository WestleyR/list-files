//
//  files.c
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

#include "files.h"

struct lf_files {
  char** paths;
  int path_count;

  int max_size;
  int max_own_len;
  int max_grup_len;

  // Machine readable output (file names only)
  bool mr_output;
  bool auto_mr_output;

  bool list_all;
};

lf_files* lf_new() {
  lf_files* ctx = (lf_files*) malloc(sizeof(lf_files));
  ctx->paths = (char**) malloc(10 * sizeof(char*));
  ctx->path_count = 0;
  ctx->list_all = false;
  ctx->max_size = 0;
  ctx->max_own_len = 0;
  ctx->max_grup_len = 0;
  ctx->mr_output = false;
  ctx->auto_mr_output = true;

  return ctx;
}

int lf_destroy(lf_files* ctx) {
  if (ctx == NULL) return -1;

  for (int i = 0; i < ctx->path_count; i++) {
    free(ctx->paths[i]);
  }
  free(ctx->paths);
  free(ctx);

  return 0;
}

int lf_add_path(lf_files* ctx, const char* path) {
  printf("DEBUG: setting path to: %s\n", path);

  ctx->paths[ctx->path_count] = (char*) malloc(strlen(path) + 2);
  strcpy(ctx->paths[ctx->path_count], path);
  ctx->path_count++;

  return 0;
}

int lf_get_max_size_from_path(lf_files* ctx) {
  if (ctx == NULL) return -1;
  if (ctx->paths == NULL) return -1;

  for (int icount = 0; icount < ctx->path_count; icount++) {
    printf("DEBUG: getting max len for path: %s\n", ctx->paths[icount]);
  
    char *full_file_path = NULL;
  
    struct max_list {
      int uid_num;
      int max_uid;
      int pid_num;
      int max_pid;
      int end;
    };
  
    // Cache up to 10 diffrant users/groups
    struct max_list ml[10];
  
    ml[0].uid_num = -1;
    ml[0].max_uid = -1;
    ml[0].pid_num = -1;
    ml[0].max_pid = -1;
    ml[0].end = 1;
  
    int mindex = 0;
  
    DIR *dr = opendir(ctx->paths[icount]);
    if (dr == NULL) {
      fprintf(stderr, "%s(): failed to open: %s\n", __func__, ctx->paths[icount]);
      return -1;
    }
  
    struct dirent *de;
    while ((de = readdir(dr)) != NULL) {
      if (ctx->list_all) {
        if ((*de->d_name == '.') || (strcmp(de->d_name, "..") == 0)) {
          continue;
        }
      }
  
      catpath(&full_file_path, ctx->paths[icount]);
      catpath(&full_file_path, de->d_name);
  
      struct stat info;
      if (lstat(full_file_path, &info) != 0) {
        perror("lstat");
        printf("error: unable to open stat on: %s\n", full_file_path);
        exit(20);
      }
  
      char* file_bytes = human_readable_bytes(info.st_size);
      if (file_bytes != NULL) {
        int file_bytes_len = strlen(file_bytes);
        if (file_bytes_len > ctx->max_size) ctx->max_size = file_bytes_len;
        free(file_bytes);
      }
  
      if (mindex >= 9) {
        struct passwd *pw;
        struct group *gr;
  
        pw = getpwuid(info.st_uid);
        gr = getgrgid(info.st_gid);
  
        int own = strlen(pw->pw_name);
        if (own > ctx->max_own_len) {
          ctx->max_own_len = own;
        }
  
        int grup = strlen(gr->gr_name);
        if (grup > ctx->max_grup_len) {
          ctx->max_grup_len = grup;
        }
      } else {
        bool match = false;
        for (int l = 0; l < mindex; l++) {
          if ((info.st_uid == ml[l].uid_num) && (info.st_gid == ml[l].pid_num)) {
            match = true;
            break;
          }
        }
  
        if (!match) {
          printf("Caching: %s : %s\n", getpwuid(info.st_uid)->pw_name, getgrgid(info.st_gid)->gr_name);
  
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
            if (ml[i].max_uid > ctx->max_own_len) ctx->max_own_len = ml[i].max_uid;
            if (ml[i].max_pid > ctx->max_grup_len) ctx->max_grup_len = ml[i].max_pid;
            if (i > 10) break;
          }
        }
      }
  
      full_file_path[0] = '\0';
    }
    closedir(dr);
  
    ml[mindex].uid_num = 0;
    ml[mindex].max_uid = 0;
    ml[mindex].pid_num = 0;
    ml[mindex].max_pid = 0;
    ml[mindex].end = 0;
  
    if (mindex < 10) {
      int i = 0;
      while (1) {
        if (ml[i].end == 0) break;
        if (ml[i].max_uid > ctx->max_own_len) ctx->max_own_len = ml[i].max_uid;
        if (ml[i].max_pid > ctx->max_grup_len) ctx->max_grup_len = ml[i].max_pid;
        i++;
        if (i > 10) break;
      }
    }
  
    ml[0].uid_num = 0;
    ml[0].max_uid = 0;
    ml[0].pid_num = 0;
    ml[0].max_pid = 0;
    ml[0].end = 0;
  
    if ((ctx->max_own_len > 20) || (ctx->max_grup_len > 20)) {
      printf("INTERNAL ERROR; recovering\n");
      ctx->max_own_len = 8;
      ctx->max_grup_len = 8;
    }
  
    free(full_file_path);
  }

  printf("DEBUG: max size: %d\n", ctx->max_size);
  printf("DEBUG: max own len: %d\n", ctx->max_own_len);
  printf("DEBUG: max grp len: %d\n", ctx->max_grup_len);

  return 0;
}

int lf_print(lf_files* ctx) {



  return 0;
}

char* human_readable_bytes(double bytes) {
  int i = 0;
  char* buf;
  float size = bytes;
  buf = (char*) malloc(10 * sizeof(char));
  const char* units[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};

  while (size > 1024) {
    size /= 1024;
    i++;
  }
  sprintf(buf, "%.*f %s", i, size, units[i]);

  return buf;
}

// vim: tabstop=2 shiftwidth=2 expandtab autoindent softtabstop=0
