// created by: WestleyR
// email: westleyr@nym.hush.com
// https://github.com/WestleyR/list-files
// date: Feb 9, 2019
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

#define BOLDRED "\033[1m\033[31m"     // bold red
#define BOLDGREEN "\033[1m\033[32m"   // bold green
#define BOLDYELLOW "\033[1m\033[33m"  // bold yellow
#define BOLDBLUE "\033[1m\033[34m"    // bold blue
#define BOLDMAGENTA "\033[1m\033[35m" // bold magenta
#define BOLDCYAN "\033[1m\033[36m"    // bold cyan
#define BOLDWHITE "\033[1m\033[37m"   // bold white
#define COLORRESET "\033[0m"          // reset

#define SCRIPT_VERSION "v1.0.0-beta-2, Feb 9, 2019"

int main(int argc, char** argv) {

    struct dirent *de;
    DIR *dr;
    struct stat sb;
    struct stat s;
    char* fileName;

    if (argc <= 1) {
        fileName = ".";
        dr = opendir(".");
    } else {
        fileName = argv[1];
        dr = opendir(argv[1]);
    }

    if (stat(fileName, &s) != 0) {
        printf("lf: ERROR: When opening file: %s\n", fileName);
    }

    if (access(fileName, F_OK) != -1) {
        if (access(fileName, R_OK) == -1) {
            fprintf(stderr, "lf: %s: Not readable.\n", fileName);
            exit(1);
        }
//        if (s.st_mode & S_IFREG) {
//            printf("FILE\n");
//        }
    } else {
        fprintf(stderr, "lf: cannot access %s: No such file or directory\n", fileName);
        exit(1);
    }


    // Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
    // for readdir()

    while ((de = readdir(dr)) != NULL) {
        if ((*(*de).d_name != '.') && (strcmp(de->d_name, "..") != 0)) {
            if (de->d_type != DT_REG) {
                printf("  %s%s%s   ", BOLDBLUE, de->d_name, COLORRESET);
            } else if (stat(de->d_name, &sb) == 0 && sb.st_mode & S_IXUSR) {
                printf("  %s%s%s   ", BOLDGREEN, de->d_name, COLORRESET);
            } else if (access(de->d_name, W_OK) != 0) {
                printf("  %s%s%s   ", BOLDYELLOW, de->d_name, COLORRESET);
            } else if (access(de->d_name, R_OK) != 0) {
                printf("  %s%s%s   ", BOLDYELLOW, de->d_name, COLORRESET);
            } else {
                printf("  %s   ", (*de).d_name);
            }
            printf("\n");
        }
    }
    closedir(dr);

    return 0;
}

//
// End main-lf.c
//
