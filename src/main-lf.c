// created by: WestleyR
// email: westleyr@nym.hush.com
// https://github.com/WestleyR/list-files
// date: Jul 13, 2019
// version-1.0.0
//
// The Clear BSD License
//
// Copyright (c) 2019 WestleyR
// All rights reserved.
//
// This software is licensed under a Clear BSD License.
//

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>

#define BOLDRED "\033[1m\033[31m"     // bold red
#define BOLDGREEN "\033[1m\033[32m"   // bold green
#define BOLDYELLOW "\033[1m\033[33m"  // bold yellow
#define BOLDBLUE "\033[1m\033[34m"    // bold blue
#define BOLDMAGENTA "\033[1m\033[35m" // bold magenta
#define BOLDCYAN "\033[1m\033[36m"    // bold cyan
#define BOLDWHITE "\033[1m\033[37m"   // bold white
#define COLORRESET "\033[0m"          // reset

#define SCRIPT_VERSION "v1.0.0-beta-18, Jul 13, 2019"

char *script_name;
char *base_path = NULL;

void help_menu() {
    printf("Usage:\n");
    printf("  %s [option] <path>\n", script_name);
    printf("\n");
    printf("Permisions:\n");
    printf("  - = file\n");
    printf("  d = directory\n");
    printf("  r = readable\n");
    printf("  w = writable\n");
    printf("  x = executable\n");
    printf("\n");
    return;
}

void version_print() {
    printf("%s\n", SCRIPT_VERSION);
    return;
}

char* readable_fs(double size, char *buf) {
    int i = 0;
    const char* units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    while (size > 1024) {
        size /= 1024;
        i++;
    }
    sprintf(buf, "%.*f %s", i, size, units[i]);
    return(buf);
}

int ends_ext(const char *string, const char* ext) {
  string = strrchr(string, '.');

  if (string != NULL) {
      return(strcmp(string, ext));
  }

  return(-1);
}

int is_zip_file(const char* file) {
    if (ends_ext(file, ".gz") == 0) {
        return(0);
    } else if (ends_ext(file, ".lz4") == 0) {
        return(0);
    } else if (ends_ext(file, ".zip") == 0) {
        return(0);
    }

    return(-1);
}

int file_info(const char* file_path) {
    struct stat sb;
    struct stat info;
    char buf[10];
    char full_file_path[256];
    full_file_path[0] = '\0';
    strcat(full_file_path, base_path);
    strcat(full_file_path, file_path);

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

    struct passwd *pw = getpwuid(info.st_uid);
    struct group *gr = getgrgid(info.st_gid);

    printf("  %-8s : ", pw->pw_name);
    printf("%-8s ", gr->gr_name);
    printf(" %-12s ", readable_fs(info.st_size, buf));

    if (S_ISLNK(info.st_mode)) {
        char symlink_path[256];
        ssize_t len = readlink(full_file_path, symlink_path, sizeof(symlink_path));
        if (len != -1) {
            symlink_path[len] = '\0';
        } else {
            printf("unable to fine link");
        }
        printf ("  %s%s  %s->  %s\n", BOLDCYAN, file_path, COLORRESET, symlink_path);
    } else if (S_ISDIR(info.st_mode)) {
        printf("  %s%s%s\n", BOLDBLUE, file_path, COLORRESET);
    } else if (stat(full_file_path, &sb) == 0 && sb.st_mode & S_IXUSR) {
        printf("  %s%s%s\n", BOLDGREEN, file_path, COLORRESET);
    } else if (is_zip_file(full_file_path) == 0) {
        printf("  %s%s%s\n", BOLDRED, file_path, COLORRESET);
    } else if (access(full_file_path, R_OK) != 0) {
        printf("  %s%s%s\n", BOLDMAGENTA, file_path, COLORRESET);
    } else {
        printf("  %s\n", file_path);
    }

    return(0);
}

// Will list files in a directory
int list_files(const char* list_path, int list_all, int mr_list) {
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
            printf("%s\n", de->d_name);
        } else {
            if (file_info(de->d_name) != 0) {
                printf("ERROR: while listing file: %s\n", de->d_name);
            }
        }
    }
    closedir(dr);

    return(0);
}

void add_char_to_string(char* s, char c) {
    int len = strlen(s);
    s[len] = c;
    s[len+1] = '\0';
}

int add_slash(char* path) {
    int len = strlen(path);
    if (path[len-1] != '/') {
        add_char_to_string(path, '/');
    }

    return(0);
}

int prep_list(const char *file_path, int list_all, int mr_list) {
    struct stat s;
    char* path = strdup(file_path);

    if (stat(path, &s) != 0) {
        printf("%s: unable to open: %s\n", script_name, path);
        return(1);
    }

    if (S_ISREG(s.st_mode)) {
        if (mr_list != 0) {
            printf("%s\n", path);
        }
        if (file_info(file_path) != 0) {
            printf("error while listing file\n");
        }
        return(0);
    }
    add_slash(path);
    base_path = strdup(path);

    if (access(file_path, F_OK) != -1) {
        if (access(file_path, R_OK) == -1) {
            fprintf(stderr, "lf: %s: Not readable.\n", path);
            exit(1);
        }
    } else {
        fprintf(stderr, "lf: cannot access %s: No such file or directory\n", path);
        exit(1);
    }

    if (list_files(path, list_all, mr_list) != 0) {
        printf("There was a problome listing files\n");
        exit(1);
    }

    return(0);
}

int main(int argc, char** argv) {
    script_name = argv[0];

    char* file_path;
    int arg_list = 0;

    // -a option
    int list_all = 0;

    // mechine readable output
    int mr_list = 0;

    file_path = "./";
    base_path = "";

    for (int i=1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0) {
            list_all = 1;
        } else if ((strcmp(argv[i], "-1") == 0) || (strcmp(argv[i], "-m") == 0)) {
            mr_list = 1;
        } else if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
            help_menu();
            return(0);
        } else if ((strcmp(argv[i], "-v") == 0) || (strcmp(argv[i], "--version") == 0)) {
            version_print();
            return(0);
        } else if ((strstr(argv[i], "--") != argv[i]) && (strstr(argv[i], "--") != argv[i])) {
            file_path = argv[i];
            prep_list(file_path, list_all, mr_list);
            arg_list = 1;
        }
    }

    if (arg_list == 0) {
        prep_list(file_path, list_all, mr_list);
    }

    return(0);
}

//
// End main-lf.c
//
