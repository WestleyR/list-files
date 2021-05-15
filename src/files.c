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

  bool rel_output;
  bool print_color;
  bool list_all;
  bool sort_by_date;
};

struct date_sorter {
  time_t file_date;
};

lf_files* lf_new() {
  lf_files* ctx = (lf_files*) malloc(sizeof(lf_files));
  ctx->paths = (char**) malloc(10 * sizeof(char*));
  // TODO: use path_count and remalloc if needed
  ctx->path_count = 0;
  ctx->max_size = 0;
  ctx->max_own_len = 0;
  ctx->max_grup_len = 0;
  ctx->list_all = false;
  ctx->mr_output = false;
  ctx->rel_output = false;
  ctx->auto_mr_output = true;
  ctx->print_color = true;
  ctx->sort_by_date = false;

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
  ctx->paths[ctx->path_count] = (char*) malloc(strlen(path) + 2);
  strcpy(ctx->paths[ctx->path_count], path);
  ctx->path_count++;

  return 0;
}

int lf_set_sort_by_date(lf_files* ctx, bool sort_by_date) {
  ctx->sort_by_date = sort_by_date;
  return 0;
}

int lf_set_print_all(lf_files* ctx, bool print_all) {
  ctx->list_all = print_all;
  return 0;
}

int lf_set_print_rel_path(lf_files* ctx, bool rel_path) {
  ctx->rel_output = rel_path;
  return 0;
}

int lf_set_print_mr_output(lf_files* ctx, bool mr_output) {
  ctx->mr_output = mr_output;
  return 0;
}

int lf_set_print_color(lf_files* ctx, bool print_color) {
  ctx->print_color = print_color;
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

// TODO: this function could be cleaner
int lf_get_max_size_from_path(lf_files* ctx) {
  if (ctx == NULL) return -1;
  if (ctx->paths == NULL) return -1;

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

  for (int icount = 0; icount < ctx->path_count; icount++) {
    struct stat info;
    if (lstat(ctx->paths[icount], &info) != 0) {
      // File does not exist
      continue;
    }

    if (!S_ISDIR(info.st_mode)) {
      // Is a normal file

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
    } else {
      // Is a directory
      DIR *dr = opendir(ctx->paths[icount]);
      if (dr == NULL) {
        // Since there was an error opening the dir, check what kind of error and print it

        // Do we have read perm?
        if (access(ctx->paths[icount], R_OK) != 0) {
          fprintf(stderr, "lf: %s: Permission denied\n", ctx->paths[icount]);
          return 13;
        }

        // Some other error
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

        char* full_file_path = NULL;
        catpath(&full_file_path, ctx->paths[icount]);
        catpath(&full_file_path, de->d_name);

        struct stat info;
        if (lstat(full_file_path, &info) != 0) {
          perror("lstat");
          printf("error: unable to open stat on: %s\n", full_file_path);
          exit(20);
        }

        free(full_file_path);

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
      }
      closedir(dr);
    }
  }

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

  return 0;
}

char* get_filedate(struct stat finfo) {
  char* ret;
  ret = (char*) malloc(20 * sizeof(char));
  if (ret == NULL) {
    perror("malloc");
    return NULL;
  }

  struct tm* p = localtime(&finfo.st_mtime);

  // For year-month-day
  strftime(ret, 1000, "%Y-%m-%d", p);

  return ret;
}

// find_link will return an allocated pointer for the linked file.
// Returned pointer must be freed.
// TODO: add null checks where called.
char* find_link(const char* path) {
  char* symlink_path = NULL;
  size_t link_len;
  size_t init_size = 80;

  // Make sure to only malloc enought, and not too much
  while (true) {
    free(symlink_path);
    symlink_path = (char*) malloc(init_size);
    link_len = readlink(path, symlink_path, init_size);

    if (link_len == -1) {
      // Error
      return NULL;
    }

    if (link_len < init_size) {
      break;
    }
    init_size += 100;
  }
  symlink_path[link_len] = '\0';

  return symlink_path;
}

bool iszip(const char* filename) {
  char* ext = strrchr(filename, '.');

  if (ext != NULL) {
    if (strcmp(ext, ".gz") == 0) {
      return true;
    } else if (strcmp(ext, ".lz4") == 0) {
      return true;
    } else if (strcmp(ext, ".tgz") == 0) {
      return true;
    } else if (strcmp(ext, ".zip") == 0) {
      return true;
    }
  }

  return false;
}

int list_file_info(lf_files* ctx, const char* filepath, const char* filename, bool is_file) {
  struct stat sb;
  struct stat info;

  char *full_file_path = NULL;

  char *print_name = NULL;

  if (ctx->rel_output) {
    catpath(&print_name, filepath);
    catpath(&print_name, filename);
  } else {
    print_name = strdup(filename);
  }

  if (!is_file) {
    catpath(&full_file_path, filepath);
  }
  catpath(&full_file_path, filename);

  if (lstat(full_file_path, &info) != 0) {
    perror("lstat");
    printf("error: unable to open stat on: %s\n", filepath);
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
    printf("  %*d", ctx->max_own_len, info.st_uid);
  } else {
    printf("  %*s", ctx->max_own_len, pw->pw_name);
  }

  struct group *gr = getgrgid(info.st_gid);
  if (gr == NULL) {
    printf("  %-*d ", ctx->max_grup_len, info.st_gid);
  } else {
    printf("  %-*s ", ctx->max_grup_len, gr->gr_name);
  }
#endif

  char* file_date = get_filedate(info);
  if (file_date == NULL) {
    printf(" %s ", "0000-00-00");
  } else {
    printf(" %s ", file_date);
    free(file_date);
  }

  char* file_bytes = human_readable_bytes(info.st_size);
  if (file_bytes != NULL) {
    printf(" %-*s", ctx->max_size, file_bytes);
    free(file_bytes);
  } else {
    printf(" %-*s", 8, "unknown");
  }

  if (ctx->print_color) {
    if (S_ISLNK(info.st_mode)) {
      char* link_path = find_link(full_file_path);

      char* full_link_path = NULL;

      if (link_path[0] == '/') {
        catpath(&full_link_path, link_path);
      } else {

        if (is_file) {
          char* path_base = dirname((char*)filepath);
          catpath(&full_link_path, path_base);
        } else {
          catpath(&full_link_path, filepath);
        }

        catpath(&full_link_path, link_path);
      }

      if (access(full_link_path, F_OK) != 0) {
        // If the link is broken
        printf("  %s%s%s  ->  %s%s%s\n", BBOLDRED, print_name, COLORRESET, BBOLDRED, link_path, COLORRESET);
      } else {
        printf("  %s%s%s  ->  %s\n", BOLDCYAN, print_name, COLORRESET, link_path);
      }
      free(link_path);
      free(full_link_path);
    } else if (S_ISDIR(info.st_mode)) {
      printf("  %s%s%s\n", BOLDBLUE, print_name, COLORRESET);
    } else if (stat(full_file_path, &sb) == 0 && sb.st_mode & S_IXUSR) {
      printf("  %s%s%s\n", BOLDGREEN, print_name, COLORRESET);
    } else if (iszip(full_file_path)) {
      printf("  %s%s%s\n", BOLDRED, print_name, COLORRESET);
    } else if (access(full_file_path, R_OK) != 0) {
      printf("  %s%s%s\n", BOLDMAGENTA, print_name, COLORRESET);
    } else {
      printf("  %s\n", print_name);
    }
  } else {
    if (S_ISLNK(info.st_mode)) {
      char* link_path = find_link(full_file_path);
      printf("  %s  ->  %s\n", print_name, link_path);
      free(link_path);
    } else {
      printf("  %s\n", print_name);
    }
  }

  free(full_file_path);
  free(print_name);

  return 0;
}

bool compare_date(time_t date1, time_t date2) {
  if (difftime(date1, date2) < 0) {
    return true;
  }
  return false;
}

int lf_print(lf_files* ctx) {
  int rc = 0;
  for (int i = 0; i < ctx->path_count; i++) {
    struct stat info;
    if (lstat(ctx->paths[i], &info) != 0) {
      fprintf(stderr, "lf: %s: No such file or directory.\n", ctx->paths[i]);
      rc = 2;
      continue;
    }

    if (!S_ISDIR(info.st_mode)) {
      // Is a normal file
      if (ctx->mr_output) {
        if (ctx->rel_output) {
          char r_path[256];
          r_path[0] = '\0';
          strcpy(r_path, ctx->paths[i]);
          strcat(r_path, ctx->paths[i]);
          printf("%s\n", r_path);
        } else {
          printf("%s\n", ctx->paths[i]);
        }
      } else {
        if (list_file_info(ctx, ctx->paths[i], ctx->paths[i], true) != 0) {
          printf("ERROR: while listing file: %s\n", ctx->paths[i]);
        }
      }
    } else {
      // Is a directory

      // Check if the dir is readable first.
      if (access(ctx->paths[i], R_OK) != 0) {
        rc = 13;
        continue;
      }

      struct dirent **namelist;

      int n = scandir(ctx->paths[i], &namelist, 0, alphasort);
      if (n < 0) {
        perror("scandir");
        continue;
      }

      // First generate the indexs for the files. We will sort this later if needed.
      int indexs[n+1];
      for (int f = 0; f < n; f++) {
        if (ctx->list_all == 0) {
          if ((namelist[f]->d_name[0] == '.') || (strcmp(namelist[f]->d_name, "..") == 0)) {
            indexs[f] = -1;
            continue;
          }
        }
        indexs[f] = f;
      }

      // If we want to sort by date
      if (ctx->sort_by_date) {
        struct date_sorter time_sort[n+1];

        // Add the filedates to an array, then sort it with our indexs.
        for (int f = 0; f < n; f++) {
          if (ctx->list_all == 0) {
            if ((namelist[f]->d_name[0] == '.') || (strcmp(namelist[f]->d_name, "..") == 0)) {
              continue;
            }
          }

          char* full_path = NULL;
          catpath(&full_path, ctx->paths[i]);
          catpath(&full_path, namelist[f]->d_name);

          struct stat info;
          if (lstat(full_path, &info) != 0) {
            perror(full_path);
            printf("FOOBAR\n");
            continue;
          }

          struct tm* p = localtime(&info.st_mtime);
          time_t t = mktime(p);
          time_sort[f].file_date = t;

          free(full_path);
        }

        // Now sort the array, and also sort the indexs
        bool sorting = true;
        while (sorting) {
          sorting = false;

          for (int s = 0; s < sizeof(time_sort)/sizeof(time_sort[0]) - 2; s++) {
            if (indexs[s] == -1) {
              continue;
            }
            time_t current = time_sort[s].file_date;

            // Find the next valid file, this may be empty for a hidden file
            int next_index = s + 1;
            while (indexs[next_index] == -1) next_index++;
            time_t next = time_sort[next_index].file_date;

            //            printf("INFO: %d -> %d\n", indexs[s], indexs[s+1]);
            //            printf("INFO: %d -> %d\n", s, next_index);
            //            printf("\n\n");

            if (compare_date(current, next)) {
              // Swap the index
              time_t tmp = current;
              int tmp_index = indexs[s];

              time_sort[s].file_date = next;
              indexs[s] = indexs[s+1];

              time_sort[s+1].file_date = tmp;
              indexs[s+1] = tmp_index;

              sorting = true;
            }
          }
        }
      }

      // Finally print the files in the order of indexs
      for (int s = 0; s < n; s++) {
        if (indexs[s] == -1) {
          continue;
        }

        if (ctx->mr_output) {
          if (ctx->rel_output) {
            char* r_path = NULL;
            catpath(&r_path, ctx->paths[i]);
            catpath(&r_path, namelist[indexs[s]]->d_name);
            printf("%s\n", r_path);
            free(r_path);
          } else {
            printf("%s\n", namelist[indexs[s]]->d_name);
          }
        } else {
          if (list_file_info(ctx, ctx->paths[i], namelist[indexs[s]]->d_name, false) != 0) {
            printf("ERROR: while listing file: %s\n", namelist[indexs[s]]->d_name);
          }
        }
        free(namelist[indexs[s]]);
      }
      free(namelist);
    }
  }

  return rc;
}

// vim: tabstop=2 shiftwidth=2 expandtab autoindent softtabstop=0
