// created by: WestleyR
// email: westleyr@nym.hush.com
// https://github.com/WestleyR/list-files
// date: Nov 16, 2019
// version-1.2.0
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
#include <getopt.h>

#ifndef WITHOUT_NAME_GROUP_OUTPUT
#include <pwd.h>
#endif

#ifndef COMMIT_HASH
#define COMMIT_HASH "unknown"
#endif

#define BOLDRED "\033[1m\033[31m"     // bold red
#define BOLDGREEN "\033[1m\033[32m"   // bold green
#define BOLDYELLOW "\033[1m\033[33m"  // bold yellow
#define BOLDBLUE "\033[1m\033[34m"    // bold blue
#define BOLDMAGENTA "\033[1m\033[35m" // bold magenta
#define BOLDCYAN "\033[1m\033[36m"    // bold cyan
#define BOLDWHITE "\033[1m\033[37m"   // bold white
#define COLORRESET "\033[0m"          // reset

#define SCRIPT_VERSION "v1.2.0-beta, Nov 16, 2019"

char *script_name;
char *base_path = NULL;

#ifndef WITHOUT_NAME_GROUP_OUTPUT
// For auto ajusting formatting
int max_own_len = 0;
int max_grup_len = 0;
#endif

int max_size = 0;

// machine readable output
int mr_list = 0;

// reletave path
int rel_path = 0;

// no color option
int no_color_print = 0;

// abs path
int abs_path = 0;

void help_menu() {
    printf("Usage:\n");
    printf("  %s [option] <path>\n", script_name);
    printf("\n");
    printf("Options:\n");
    printf("  -a             list all files\n");
    printf("  -p             list files with relative path\n");
    printf("  -1, -m         only print file names (mr), color will be off\n");
    printf("  -c, --color=   color output, options: on,off,auto\n");
    printf("  -h, --help     print help menu\n");
    printf("  -C, --commit   print the github commit\n");
    printf("  -V, --version  print version\n");
    printf("\n");
    printf("Permisions:\n");
    printf("  - = file, if its the first option,\n");
    printf("      otherwise, no permission\n");
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

void version_commit() {
    printf("%s\n", COMMIT_HASH);
    return;
}

char* readable_fs(double bytes) {
    int i = 0;
    char* buf;
    float size = bytes;
    buf = (char*) malloc(sizeof(bytes));
    const char* units[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};

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

#ifdef WITHOUT_NAME_GROUP_OUTPUT
    printf("  %4d", info.st_uid);
    printf("  %4d ", info.st_gid);
#else
    struct passwd *pw = getpwuid(info.st_uid);
    struct group *gr = getgrgid(info.st_gid);
    printf("  %*s", max_own_len, pw->pw_name);
    printf("  %*s ", max_grup_len, gr->gr_name);
#endif

    char* file_bytes = readable_fs(info.st_size);
    printf(" %-*s", max_size, file_bytes);
    free(file_bytes);

    if (no_color_print == 0) {
        if (S_ISLNK(info.st_mode)) {
            char symlink_path[126];
            ssize_t len = readlink(full_file_path, symlink_path, sizeof(symlink_path));
            if (len != -1) {
                symlink_path[len] = '\0';
            } else {
                printf("unable to fine link");
            }
            printf("  %s%s%s  ->  %s\n", BOLDCYAN, print_name, COLORRESET, symlink_path);
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
    } else {
        if (S_ISLNK(info.st_mode)) {
            char symlink_path[126];
            ssize_t len = readlink(full_file_path, symlink_path, sizeof(symlink_path));
            if (len != -1) {
                symlink_path[len] = '\0';
            } else {
                printf("unable to fine link");
            }
            printf ("  %s  ->  %s\n", print_name, symlink_path);
        } else {
            printf("  %s\n", print_name);
        }
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
                char r_path[200];
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

#ifndef WITHOUT_NAME_GROUP_OUTPUT
// Will loop files in a directory, and get the max lenth for all the permamiters
int max_len_files(const char* list_path, int list_all) {
    DIR *dr;
    struct dirent *de;
    struct stat info;

    char full_file_path[256];
    full_file_path[0] = '\0';

    struct max_list {
        int uid_num;
        int max_uid;
        int pid_num;
        int max_pid;
        int end;
    };

    struct max_list ml[10];

    ml[0].uid_num = 0;
    ml[0].max_uid = 0;
    ml[0].pid_num = 0;
    ml[0].max_pid = 0;
    ml[0].end = 1;

    int mindex = 0;
    int match = 0;

    dr = opendir(list_path);
    while ((de = readdir(dr)) != NULL) {
        if (list_all == 0) {
            if ((*de->d_name == '.') || (strcmp(de->d_name, "..") == 0)) {
                continue;
            }
        }

        strcat(full_file_path, base_path);
        strcat(full_file_path, de->d_name);

        if (lstat(full_file_path, &info) != 0) {
            perror("lstat");
            printf("error: unable to open stat on: %s\n", full_file_path);
            exit(20);
        }

        char* file_bytes = readable_fs(info.st_size);
        int file_bytes_len = strlen(file_bytes);
        if (file_bytes_len > max_size) max_size = file_bytes_len;

        if (mindex >= 10) {
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
            for (int l = 0; l < 10; l++) {
                if ((info.st_uid == ml[l].uid_num) && (info.st_gid == ml[l].pid_num)) {
                    match = 1;
                    break;
                }
            }

            if (match != 1) {
                ml[mindex].uid_num = (int)info.st_uid;
                ml[mindex].max_uid = (int)strlen(getpwuid(info.st_uid)->pw_name);

                ml[mindex].pid_num = (int)info.st_gid;
                ml[mindex].max_pid = (int)strlen(getgrgid(info.st_gid)->gr_name);

                ml[mindex].end = 1;
                mindex++;
            }

            match = 0;

            if (mindex >= 10) {
                for (int i = 0; i < mindex; i++) {
                    if (ml[i].end == 0) break;
                    if (ml[i].max_uid > max_own_len) max_own_len = ml[i].max_uid;
                    if (ml[i].max_pid > max_grup_len) max_grup_len = ml[i].max_pid;
                    //if (i > 10) break;
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
            if (ml[i].max_uid > max_own_len) max_own_len = ml[i].max_uid;
            if (ml[i].max_pid > max_grup_len) max_grup_len = ml[i].max_pid;
            i++;
            //if (i > 10) break;
        }
    }

    ml[0].uid_num = '\0';
    ml[0].max_uid = '\0';
    ml[0].pid_num = '\0';
    ml[0].max_pid = '\0';
    ml[0].end = '\0';

    //    struct max_list ml[0] = {};
    //    ml[0] = (const struct max_list){};

    if ((max_own_len > 20) || (max_grup_len > 20)) {
        printf("INTERNAL ERROR; recovering\n");
        max_own_len = 8;
        max_grup_len = 8;
    }

    return(0);
}
#endif

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
        fprintf(stderr, "%s: %s: No such file or directory\n", script_name, path);
        exit(2);
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

#ifndef WITHOUT_NAME_GROUP_OUTPUT
    if (mr_list != 1) {
        max_len_files(path, list_all);
    }
#endif

    if (list_files(path, list_all) != 0) {
        printf("There was a problome listing files\n");
        exit(1);
    }

    return(0);
}

int main(int argc, char** argv) {
    script_name = argv[0];

    char* file_path;

    // -a option
    int list_all = 0;

    // If piped or redirected
    if (isatty(STDOUT_FILENO) == 0) {
        no_color_print = 1;
    }

    int opt = 0;

    char* color_print;
    color_print = (char*) malloc(4);

    file_path = "./";
    base_path = "";

    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"rel-path", no_argument, 0, 'p'},
        {"mr", no_argument, 0, '1'},
        {"mr", no_argument, 0, 'm'},
        {"all", no_argument, 0, 'a'},
        {"color", required_argument, 0, 'c'},
        {"version", no_argument, 0, 'V'},
        {"commit", no_argument, 0, 'C'},
        {NULL, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv,"c:1mapVhC", long_options, 0)) != -1) {
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
            case 'c':
                color_print = optarg;
                break;
            case 'h':
                help_menu();
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

    // If the --color option is set
    if (color_print[0] != '\0') {
        if (strcmp(color_print, "auto") == 0) {
            // Do nothing
        } else if (strcmp(color_print, "on") == 0) {
            no_color_print = 0;
        } else if (strcmp(color_print, "off") == 0) {
            no_color_print = 1;
        } else {
            fprintf(stderr, "%s: --color: %s: invalid argument\n", script_name, color_print);
            return(22);
        }
    }

    if (optind < argc) {
        for (int i = optind; i < argc; i++) {
            file_path = argv[i];
            prep_list(file_path, list_all);
        }
    } else {
        prep_list(file_path, list_all);
    }

    return(0);
}

//
// End main-lf.c
//
