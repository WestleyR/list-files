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

#define SCRIPT_VERSION "v1.0.0-beta-22, Jul 13, 2019"

char *script_name;
char *base_path = NULL;

int max_own_len = 0;
int max_grup_len = 0;

// mechine readable output
int mr_list = 0;

// reletave path
int rel_path = 0;

// abs path
int abs_path = 0;

void help_menu() {
    printf("Usage:\n");
    printf("  %s [option] <path>\n", script_name);
    printf("\n");
    printf("Options:\n");
    printf("  -a      list all files\n");
    printf("  -p      list files with rel path\n");
    printf("  -1, -m  only print file names\n");
    printf("  -h      print help menu\n");
    printf("  -v      print version\n");
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
    char *print_name = NULL;
    full_file_path[0] = '\0';
    strcat(full_file_path, base_path);
    strcat(full_file_path, file_path);

    if (rel_path != 0) {
        print_name = full_file_path;
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

    struct passwd *pw = getpwuid(info.st_uid);
    struct group *gr = getgrgid(info.st_gid);

    printf("  %-8s ", pw->pw_name);
    printf("%-10s ", gr->gr_name);
    printf(" %-12s", readable_fs(info.st_size, buf));

    if (S_ISLNK(info.st_mode)) {
        char symlink_path[256];
        ssize_t len = readlink(full_file_path, symlink_path, sizeof(symlink_path));
        if (len != -1) {
            symlink_path[len] = '\0';
        } else {
            printf("unable to fine link");
        }
        printf ("  %s%s  %s->  %s\n", BOLDCYAN, print_name, COLORRESET, symlink_path);
    } else if (S_ISDIR(info.st_mode)) {
        printf("  %s%s%s\n", BOLDBLUE, print_name, COLORRESET);
    } else if (stat(full_file_path, &sb) == 0 && sb.st_mode & S_IXUSR) {
        printf("  %s%s%s\n", BOLDGREEN, print_name, COLORRESET);
    } else if (is_zip_file(full_file_path) == 0) {
        printf("  %s%s%s\n", BOLDRED, print_name, COLORRESET);
    } else if (access(full_file_path, R_OK) != 0) {
        printf("  %s%s%s\n", BOLDMAGENTA, print_name, COLORRESET);
    } else {
        printf("  %s\n", print_name);
    }

    return(0);
}

// Will list files in a directory
int list_files(const char* list_path, int list_all) {
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
                char r_path[126];
                r_path[0] = '\0';
                strcat(r_path, list_path);
                strcat(r_path, de->d_name);
                printf("%s\n", r_path);
                r_path[0] = '\0';
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

// Will loop files in a directory, and get the max lenth for all the permamiters
int max_len_files(const char* list_path, int list_all) {
    DIR *dr;
    struct dirent *de;
    struct stat info;

    dr = opendir(list_path);
    while ((de = readdir(dr)) != NULL) {
        if (list_all == 0) {
            if ((*de->d_name == '.') || (strcmp(de->d_name, "..") == 0)) {
                continue;
            }
        }

//        printf("name: %s\n", de->d_name);

	    char full_file_path[256];
	    full_file_path[0] = '\0';
    	strcat(full_file_path, base_path);
	    strcat(full_file_path, de->d_name);

	    //printf("FULL: %s\n", full_file_path);

        if (lstat(full_file_path, &info) != 0) {
            perror("lstat");
            printf("error: unable to open stat on: %s\n", full_file_path);
            exit(20);
        }

        struct passwd *pw = getpwuid(info.st_uid);
        struct group *gr = getgrgid(info.st_gid);

        int own = strlen(pw->pw_name);
//        printf("own: %s: %d\n", pw->pw_name, own);
        if (own > max_own_len) {
            max_own_len = own;
        }

        int grup = strlen(gr->gr_name);
//        printf("grup: %s: %d\n", gr->gr_name, grup);
        if (grup > max_grup_len) {
            max_grup_len = grup;
        }
	    full_file_path[0] = '\0';
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

int prep_list(const char *file_path, int list_all) {
    struct stat s;
    char* path = strdup(file_path);

    if (stat(path, &s) != 0) {
        fprintf(stderr, "%s: No such file or directory\n", path);
        exit(2);
        //printf("%s: unable to open: %s\n", script_name, path);
        //return(2);
    }

    if (S_ISDIR(s.st_mode) == 0) {
        if (mr_list != 0) {
            printf("%s\n", path);
            return(0);
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
            fprintf(stderr, "%s: Not readable\n", path);
            exit(1);
        }
    } else {
        fprintf(stderr, "PANIC: cannot access %s: No such file or directory\n", path);
        exit(1);
    }

    max_len_files(path, list_all);
    printf("MAX_LEN_OWN: %d\n", max_own_len);
    printf("MAX_LEN_GROUP: %d\n", max_grup_len);

    if (list_files(path, list_all) != 0) {
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

    file_path = "./";
    base_path = "";

    for (int i=1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0) {
            list_all = 1;
        } else if ((strcmp(argv[i], "-1") == 0) || (strcmp(argv[i], "-m") == 0)) {
            mr_list = 1;
        } else if (strcmp(argv[i], "-p") == 0) {
            rel_path = 1;
//            break;
        } else if (strcmp(argv[i], "-P") == 0) {
            abs_path = 1;
        } else if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
            help_menu();
            return(0);
        } else if ((strcmp(argv[i], "-v") == 0) || (strcmp(argv[i], "--version") == 0)) {
            version_print();
            return(0);
        } else if ((strstr(argv[i], "--") != argv[i]) && (strstr(argv[i], "--") != argv[i])) {
            file_path = argv[i];
            prep_list(file_path, list_all);
            arg_list = 1;
        }
    }

    if (arg_list == 0) {
        prep_list(file_path, list_all);
    }

    return(0);
}

//
// End main-lf.c
//
