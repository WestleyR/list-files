// created by: WestleyR
// email: westleyr@nym.hush.com
// https://github.com/WestleyR/list-files
// date: Jul 6, 2019
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

#define SCRIPT_VERSION "v1.0.0-beta-14, Jul 6, 2019"

char *script_name;

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
    return buf;
}

char* concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    // TODO: in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return(result);
}

int main(int argc, char** argv) {
    script_name = argv[0];

    struct dirent *de;
    DIR *dr;
    struct stat sb;
    struct stat s;
    char* fileName;

    int listAll = 0;
    int longList = 0;

    fileName = "./";

    for (int i=1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0) {
            listAll = 1;
        } else if (strcmp(argv[i], "-l") == 0) {
            longList = 1;
        } else if (strcmp(argv[i], "--help") == 0) {
            help_menu();
            return(0);
        } else if (strcmp(argv[i], "--version") == 0) {
            version_print();
            return(0);
        } else if ((strstr(argv[i], "--") != argv[i]) && (strstr(argv[i], "--") != argv[i])) {
                fileName = argv[i];
        }
    }

    if (stat(fileName, &s) != 0) {
        printf("%s: unable to open: %s\n", script_name, fileName);
        return(1);
    }

    if (S_ISREG(s.st_mode)) {
        printf("error: %s: is a plane file.\n", fileName);
        return(1);
    }
    

    if (access(fileName, F_OK) != -1) {
        if (access(fileName, R_OK) == -1) {
            fprintf(stderr, "lf: %s: Not readable.\n", fileName);
            exit(1);
        }
    } else {
        fprintf(stderr, "lf: cannot access %s: No such file or directory\n", fileName);
        exit(1);
    }

    dr = opendir(fileName);
    while ((de = readdir(dr)) != NULL) {
        if ((*de->d_name != '.') && (strcmp(de->d_name, "..") != 0)) {

            struct stat info;
            char buf[10];
            char *full_file_path;
            full_file_path = concat(fileName, de->d_name);
            if (lstat(full_file_path, &info) != 0) {
                printf("error: unable to open stat on: %s\n", de->d_name);
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

/*            if (S_ISDIR(info.st_mode)) {
                printf("   d");
            } else if (S_ISLNK(info.st_mode)) {
                printf("   l");
            } else {
                printf("   f");
            }
            printf((info.st_mode & S_IRUSR) ? "r" : "-");
            printf((info.st_mode & S_IWUSR) ? "w" : "-");
            printf((info.st_mode & S_IXUSR) ? "x  " : "-  ");
*/

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
               printf ("  %s%s  %s->  %s\n", BOLDCYAN, de->d_name, COLORRESET, symlink_path);
            } else if (S_ISDIR(info.st_mode)) {
                printf("  %s%s%s\n", BOLDBLUE, de->d_name, COLORRESET);
            } else if (stat(full_file_path, &sb) == 0 && sb.st_mode & S_IXUSR) {
                printf("  %s%s%s\n", BOLDGREEN, de->d_name, COLORRESET);
            } else if (access(full_file_path, R_OK) != 0) {
                printf("  %s%s%s\n", BOLDMAGENTA, de->d_name, COLORRESET);
            } else {
                printf("  %s\n", de->d_name);
            }
        } else if (listAll == 1) {
            struct stat info;
            char buf[10];
            char *full_file_path;
            full_file_path = concat(fileName, de->d_name);
            if (lstat(full_file_path, &info) != 0) {
                printf("error: unable to open stat on: %s\n", de->d_name);
                exit(20);
            }

            // TODO: use 'l' for links
            printf((S_ISDIR(info.st_mode)) ? "d" : "-");
            printf((info.st_mode & S_IRUSR) ? "r" : "-");
            printf((info.st_mode & S_IWUSR) ? "w" : "-");
            printf((info.st_mode & S_IXUSR) ? "x" : "-");
            printf((info.st_mode & S_IRGRP) ? "r" : "-");
            printf((info.st_mode & S_IWGRP) ? "w" : "-");
            printf((info.st_mode & S_IXGRP) ? "x" : "-");
            printf((info.st_mode & S_IROTH) ? "r" : "-");
            printf((info.st_mode & S_IWOTH) ? "w" : "-");
            printf((info.st_mode & S_IXOTH) ? "x" : "-");

            if (S_ISDIR(info.st_mode)) {
                printf("   d");
            } else if (S_ISLNK(info.st_mode)) {
                printf("   l");
            } else {
                printf("   f");
            }
            printf((info.st_mode & S_IRUSR) ? "r" : "-");
            printf((info.st_mode & S_IWUSR) ? "w" : "-");
            printf((info.st_mode & S_IXUSR) ? "x  " : "-  ");

            struct passwd *pw = getpwuid(info.st_uid);
            struct group *gr = getgrgid(info.st_gid);

            printf("  %-8s : ", pw->pw_name);
            printf("%-8s ", gr->gr_name);

            printf(" %-10s ", readable_fs(info.st_size, buf));

           if (S_ISLNK(info.st_mode)) {
               char symlink_path[256];
               ssize_t len = readlink(full_file_path, symlink_path, sizeof(symlink_path));
               if (len != -1) {
                   symlink_path[len] = '\0';
               } else {
                   printf("unable to fine link");
               }
               printf ("  %s%s  %s->  %s\n", BOLDCYAN, de->d_name, COLORRESET, symlink_path);
            } else if (S_ISDIR(info.st_mode)) {
                printf("  %s%s%s\n", BOLDBLUE, de->d_name, COLORRESET);
            } else if (stat(full_file_path, &sb) == 0 && sb.st_mode & S_IXUSR) {
                printf("  %s%s%s\n", BOLDGREEN, de->d_name, COLORRESET);
            } else if (access(full_file_path, R_OK) != 0) {
                printf("  %s%s%s\n", BOLDMAGENTA, de->d_name, COLORRESET);
            } else {
                printf("  %s\n", de->d_name);
            }
        }
    }
    closedir(dr);

    if (longList != 0) {
        printf("\n");
    }

    return 0;
}

//
// End main-lf.c
//
