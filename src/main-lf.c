// created by: WestleyR
// email: westleyr@nym.hush.com
// https://github.com/WestleyR/list-files
// date: Feb 22, 2019
// version-1.0.0
//
// The Clear BSD License
//
// Copyright (c) 2019 WestleyR
// All rights reserved.
//
// This software is licensed under a Clear BSD License.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/ioctl.h>

#define SCRIPT_VERSION  "v1.0.0-beta-6, Feb 22, 2019"

#define SIZE_N 1024
#define SIZE_P 16
#define SIZE_T 16

typedef enum {
	INT, LONG_INT, STRING
} type_t;

typedef struct columns_t {
	int nlink_c;
	int user_c;
	int group_c;
	int size_c;
} columns_t;

typedef struct options_t {
	int flag_l;
	int flag_R;
	int flag_a;
	int flag_d;
	int flag_S;
	int flag_t;
} options_t;

typedef struct fileinfo_t {
	long int f_size;
	long long int tick;
	int n_link;
	char *perm;
	char *user;
	char *group;
	char *time;
	char *name;
} fileinfo_t;

int lf ( options_t * , char ** , int);
int _lf ( options_t * , char * , columns_t *);
int __lf ( options_t * , char * , columns_t *);
int ___lf ( options_t * , char ** , int , columns_t *);

int (filter) (const struct dirent *);
int (compare) (const struct dirent **, const struct dirent ** );
int (compar) (const void *, const void *);
int (compa) (const void *, const void *);

void printdata(struct fileinfo_t *, int n, options_t *, columns_t *, int);

void reset_col(columns_t *);
void set_c_size(columns_t *, int, char *, char *, long int);

char *getperm(char * , struct stat filestat);
int getlinks(struct stat filestat);
char *getuser(struct stat filestat);
char *getgroup(struct stat filestat);
int	getsize(struct stat filestat);
char *getdate(char * , struct stat filestat);
char *getname(char * , char * , char , int);

char* SCRIPT_NAME = NULL;

char *getcolorstring(char *, char *, char, int);
char *getprintstring(char *, type_t, void*);

int findmax(int * , int , int); 

int (compar)(const void * a , const void * b ) {
	long int c = ((fileinfo_t *)a)->f_size;
	long int d = ((fileinfo_t *)b)->f_size;

	return d - c;
}

int (compa)(const void * a , const void * b ) {
	long long int c = ((fileinfo_t *)a)->tick;
	long long int d = ((fileinfo_t *)b)->tick;
	return d - c;
}



void help_menu() {
	printf("DESCRIPTION:\n");
	printf("  simple, fast file lister - lf - list files.\n");
	printf("\n");
	printf("USAGE:\n");
	printf("  %s [option] <path>\n", SCRIPT_NAME);
	printf("\n");
	printf("OPTIONS:\n");
	printf("  -a  : list all files.\n");
	printf("  -d  : list only directorys.\n");
	printf("  -h  : print help menu and exit.\n");
	printf("  -l  : long list.\n");
	printf("  -R  : list recursively.\n");
	printf("  -S  : sort files by size.\n");
	printf("  -t  : sort by time modified.\n");
	printf("  -v  : print lf version and exit.\n");
	printf("\n");
	printf("Sorce code: https://github.com/WestleyR/list-files\n");
	exit(0);
}

void script_version() {
	printf("%s\n", SCRIPT_VERSION);
	exit(0);
}

void set_c_size(columns_t * col, int nlink, char* user, char* group , long int size) {
	int len;

	len = 0;
	while (nlink != 0) {
		len ++;
		nlink /= 10;
	}

	col->nlink_c = (len > col->nlink_c) ? len : col -> nlink_c ;

	len = 0;
	while (size != 0) {
		len ++;
		size /= 10;
	}
	col->size_c = (len > col->size_c) ? len : col -> size_c;

	len = 0;
	while (user[len++]);
	col->user_c = (len > col->user_c) ? len - 1 : col -> user_c;

	len = 0;
	while (group[len++]);
	col->group_c = (len > col->group_c) ? len - 1 : col -> group_c;

	return;

}

int (filter)(const struct dirent * direntry) {
	return (*direntry).d_name[0] != '.';
}

int (compare)(const struct dirent ** e1, const struct dirent ** e2) {
	char *s1 = (char*) (**e1).d_name;
	char *s2 = (char*) (**e2).d_name;
	char *string1 = (char *) calloc(strlen(s1) + 1 , sizeof(char));
	char *string2 = (char *) calloc(strlen(s2) + 1, sizeof(char));

	char *oldS1 = string1 , *oldS2 = string2;

	int result = 0;

	while (*s1) {
		if (*s1 == '.') {
			s1++;
			continue;
		}
		if (*s1 >= 'A' && *s1 <= 'Z') {
			*string1 = *s1 + ('a'-'A');
		} else {
			*string1=*s1;
		}
		s1++;
		string1++;
	}

	while (*s2) {
		if (*s2 == '.') {
			s2++;
			continue;
		}
		if (*s2 >= 'A' && *s2 <= 'Z') {
			*string2 = *s2 + ('a'-'A');
		} else {
			*string2 = *s2;
		}
		if (*string2 == '.' ) {
			*string2 = 'a';
		}
		s2++;
		string2++;
	}
	(*string1) = (*string2) = 0;
	result = strcmp(oldS1 , oldS2);

	free(oldS1);
	free(oldS2);
	
	return(result);

}

void reset_col ( columns_t * col ) {
	col -> nlink_c = 0;
	col -> user_c = 0;
	col -> group_c = 0;
	col -> size_c = 0;
	return;
}


void printdata(struct fileinfo_t * fileinfo , int n , options_t * opt , columns_t * col , int total) {

	int i = 0 , num_col = 0;
	struct winsize w;
	char * print_str = (char *) calloc( 1024 , sizeof(char));
	int * array_i = (int *) calloc( n , sizeof(int) );
	int isTerminal = 0;

	if (isatty(fileno(stdout))) {
		isTerminal = 1;
	}

	ioctl(0, TIOCGWINSZ, &w);

	if (opt -> flag_S) {
		qsort(fileinfo, n, sizeof(fileinfo_t), compar);
	} else if (opt -> flag_t) {
		qsort(fileinfo, n, sizeof(fileinfo_t), compa);
	}

	if (opt->flag_l == 1) {
		if (total != -1) {
			printf("total %d\n" , total);
		}
		for (i = 0 ; i < n ; i ++) {

			printf("%10s " , fileinfo[i].perm); 

			print_str = getprintstring( print_str , INT , &col->nlink_c);
			printf(print_str , fileinfo[i].n_link);

			print_str = getprintstring( print_str , STRING , &col->user_c);
			printf(print_str , fileinfo[i].user); 

			print_str = getprintstring( print_str , STRING , &col->group_c);
			printf(print_str , fileinfo[i].group); 

			print_str = getprintstring( print_str , LONG_INT , &col->size_c);
			printf(print_str , fileinfo[i].f_size); 

			printf("%s " , fileinfo[i].time); 

			print_str = getcolorstring( print_str , fileinfo[i].perm , isTerminal , opt->flag_l);
			printf(print_str , 0 , fileinfo[i].name);
			printf("\n");

			if (isTerminal) printf("\e[0;39m");

		}
		printf("\n");

	} else {
		num_col = w.ws_col;

		int n_r = 1 , flag_col = 0 , j = 0 , sum = 0;
		int * length = (int *) malloc( n * sizeof(int) );
		int * max_length = (int *) malloc( n * sizeof(int) );

		for (i = 0 ; i < n ; i++) {
			length[i] = strlen(fileinfo[i].name) + 2;
		}

		while (flag_col != 1) {

			for (i = 0 ; i < n/n_r ; i++) {
				max_length[i] = findmax(length , i , n_r);
			}

			sum = 0;
			for (i = 0 ; i < n/n_r ; i++) {
				sum += max_length[i];
			}
			sum += n/n_r;
			if (sum > num_col) {
				n_r ++;
			} else {
				flag_col = 1;
			}
		}

		for (i = 0 ; i < n ; i++) {
			array_i[i] = -1;
		}

		for (i = 0 ; i <= n_r ; i++) {
			for (j = 0 ; j <= n/n_r ; j++) {
				if ((j * n_r + i) < n && array_i[j * n_r + i] == -1) {
					print_str = getcolorstring( print_str , fileinfo[j * n_r + i].perm , isTerminal , opt->flag_l);
					printf(print_str , -max_length[j] , fileinfo[ j * n_r + i ].name);
					array_i[j*n_r + i] = 0;
				}
			}
			if (i != n_r) {
				printf("\n");
			}
		}

		free(length);
		free(max_length);

	}
	free(print_str);
	free(array_i);
	return;
}

int findmax(int *array , int col , int row ) {

	int i = 0 , max = array[col * row];
	for (i = 0 ; i < row ; i++) {
		if (array[col * row + i] > max) {
			max = array[col*row + i];
		}
	}

	return(max);

}
 
char *getprintstring(char *print_str , type_t type , void * value) {
	switch (type) {
		case INT:
			sprintf(print_str , "%%%dd " , *((int*) value) );
			return print_str;
			break;
		case LONG_INT:
			sprintf(print_str , "%%%dld " , *((int*) value) );
			return print_str;
			break;
		case STRING:
			sprintf(print_str , "%%%ds " , *((int*) value) );
			return print_str;
			break;
	}
	return(NULL);
}

char *getcolorstring(char * print_str , char * perm , char isTerminal , int flag_l) {

	if( isTerminal == 0 ) {
		if (flag_l == 1) {
			strcpy(print_str , "%*s");
		} else {
			strcpy(print_str , "%*s\n");
		}
		return(print_str);
	}

	if (perm[0] == 'd') {
		sprintf(print_str , "\e[1;34m%%*s\e[0;39m");
	} else if (perm[0] == 'l') {
		sprintf(print_str , "\e[1;36m%%*s\e[0;39m");
	} else if (perm[0] == 'p') {
		sprintf(print_str , "\e[1;33;40m%%*s\e[0;39m");
	} else if( perm[3] == 'x' || perm[6] == 'x' || perm[9] == 'x' ) {
		sprintf(print_str , "\e[1;92m%%*s\e[0;39m");
	} else {
		sprintf(print_str , "\e[0;39m%%*s\e[0;39m");
	}
	return(print_str);
}

int lf (options_t * opt , char ** arg , int count_arg) {
	int i = 0;
	int count_arg_f = 0 , count_arg_d = 0;
	char ** arg_d = NULL , ** arg_f = NULL;

	struct stat fileStat;

	columns_t * col = (columns_t *) calloc(1 , sizeof(columns_t));

	arg_d = (char **) calloc (count_arg , sizeof(char *));
	arg_f = (char **) calloc (count_arg , sizeof(char *));

	if (opt -> flag_d == 0 && count_arg == 0) {

		_lf(opt , "." , col);

		free(col);
		free(arg_d);
		free(arg_f);
		return 0;
	} else if (opt -> flag_d == 1 && count_arg == 0) {
		free(arg_d);

		arg_d = (char **) calloc ( 1 , sizeof(char *) );
		arg_d[0] = (char *) calloc ( SIZE_P , sizeof(char) );
		strcpy(arg_d[0] , ".");

		___lf(opt , arg_d , 1 , col);

		free(col);
		free(arg_d[0]);
		free(arg_d);
		free(arg_f);
		return 0;
	}

	for (i = 0 ; i < count_arg ; i++) {
		reset_col(col);
		if (lstat(arg[i], &fileStat)) {
			perror("lf: Cannot access: ");
			break;
		}

		if (S_ISDIR(fileStat.st_mode)) {
			arg_d[count_arg_d++] = arg[i];
		} else {
			arg_f[count_arg_f++] = arg[i];
		}

	}

	if (opt -> flag_d == 1) {
		___lf(opt , arg , count_arg , col);
	} else {
		if (count_arg_f != 0) {
			___lf(opt , arg_f , count_arg_f  , col);
		}

		for (i = 0 ; i < count_arg_d ; i++) {
			_lf(opt , arg_d[i] , col);
		}
	}

	free(col);
	free(arg_d);
	free(arg_f);
	return 0;
}
 
int _lf( options_t * opt , char * path , columns_t * col ) {

	struct dirent **namelist = NULL;
	struct fileinfo_t *fileinfo = NULL;
	struct stat fileStat;
	char * cwd = NULL;
	char ** list_d = NULL;
	int n = 0, i = 0, count_d = 0;
	int total = 0;

	if (opt->flag_a == 0) {
		n = scandir(path, &namelist, filter , compare);
	} else {
		n = scandir(path, &namelist, NULL , compare);
	}

	list_d = (char **) calloc (n , sizeof(char *));

	cwd   = (char *) calloc(2048 , sizeof(char) );

	if (getcwd(cwd, 2048) == NULL) {
		perror("getcwd() : ");
	}

	if (chdir(path)) {
		perror("Error");
//		printf("A error has occured.\n");
	}

	if (n < 0) {
		perror("lf: cannot open directory ");
	} else {
		fileinfo = (fileinfo_t *) calloc(n , sizeof(fileinfo_t));
		for (i = 0 ; i < n ; i++) {
			if (lstat(namelist[i]->d_name , &fileStat) < 0) {
				perror(namelist[i]->d_name);
				return -1;
			}

			fileinfo[i].perm = (char *) calloc( SIZE_P, sizeof(char));
			fileinfo[i].time = (char *) calloc( SIZE_T, sizeof(char));
			fileinfo[i].name = (char *) calloc( SIZE_N, sizeof(char));

			fileinfo[i].perm = getperm (fileinfo[i].perm, fileStat);
			fileinfo[i].name = getname (fileinfo[i].name, namelist[i]->d_name, fileinfo[i].perm[0], opt->flag_l);
			fileinfo[i].n_link = getlinks(fileStat);
			fileinfo[i].user = getuser (fileStat);
			fileinfo[i].group = getgroup(fileStat);
			fileinfo[i].f_size = getsize (fileStat);
			fileinfo[i].time = getdate (fileinfo[i].time , fileStat);
			fileinfo[i].tick = (long long int) fileStat.st_mtime;

			total += fileStat.st_blocks;

			set_c_size(col, fileinfo[i].n_link, fileinfo[i].user, fileinfo[i].group, fileinfo[i].f_size);

			if (fileinfo[i].perm[0] == 'd') {
				list_d[count_d] = (char *) malloc( SIZE_N);
				strcpy(list_d[count_d], fileinfo[i].name);
				count_d ++;
			}
		}
		printdata(fileinfo, n, opt, col, total/2);

		if (opt -> flag_R == 1) {			
			for (i = 0; i < count_d; i++) {
				if (strcmp(".", list_d[i]) && strcmp("..", list_d[i])) {

					char ** temp_d = (char **) calloc (1 , sizeof(char *));
					temp_d[0] = (char *) malloc (SIZE_P * sizeof(char *));
					strcpy(temp_d[0], list_d[i]);
	
					lf(opt, temp_d, 1);
	
					free(temp_d[0]);
					free(temp_d);
				}
			}
		}
	}

	if (chdir(cwd)) {
		perror("Error");
	}

	for (i = 0 ; i < n ; i++) {
		free(namelist[i]);
		free(fileinfo[i].perm);
		free(fileinfo[i].time);
		free(fileinfo[i].name);
	}
	for (i = 0; i < count_d; i++) {
		free(list_d[i]);
	}
	free(namelist);
	free(fileinfo);
	free(list_d);	
	free(cwd);
	return 0;
}

int ___lf( options_t * opt , char ** files , int count_arg , columns_t * col) {

	struct fileinfo_t *fileinfo = NULL;
	struct stat fileStat;
	char * cwd = NULL;
	int i = 0;

	reset_col(col);

	fileinfo = (fileinfo_t *) calloc(count_arg , sizeof(fileinfo_t));
	for (i = 0; i < count_arg; i++) {
		if (lstat(files[i], &fileStat)) {
			perror(files[i]);
			return -1;
		}

		fileinfo[i].perm = (char *) calloc( SIZE_P , sizeof(char));
		fileinfo[i].time = (char *) calloc( SIZE_T , sizeof(char));
		fileinfo[i].name = (char *) calloc( SIZE_N , sizeof(char));

		fileinfo[i].perm = getperm (fileinfo[i].perm, fileStat);
		fileinfo[i].name = getname (fileinfo[i].name, files[i], fileinfo[i].perm[0] , opt->flag_l );
		fileinfo[i].n_link = getlinks(fileStat);
		fileinfo[i].user = getuser (fileStat);
		fileinfo[i].group = getgroup(fileStat);
		fileinfo[i].f_size = getsize (fileStat);
		fileinfo[i].time = getdate (fileinfo[i].time , fileStat);
		fileinfo[i].tick = (long long int) fileStat.st_mtime;

		set_c_size(col, fileinfo[i].n_link, fileinfo[i].user, fileinfo[i].group, fileinfo[i].f_size);

	}
	printdata(fileinfo, count_arg, opt, col, -1);

	
	for (i = 0 ; i < count_arg ; i++) {
		free(fileinfo[i].perm);
		free(fileinfo[i].time);
		free(fileinfo[i].name);
	}
	free(fileinfo);	
	free(cwd);
	return 0;
}

int __lf (options_t * opt, char *path, columns_t * col) {

	struct dirent **namelist = NULL;
	struct fileinfo_t *fileinfo = NULL;
	struct stat fileStat;
	int i = 0;

	fileinfo = (fileinfo_t *) calloc(1 , sizeof(fileinfo_t));
	if (lstat(path , &fileStat)) {
		perror(namelist[i] -> d_name);
		return -1;
	}
	fileinfo[i].perm = (char *) calloc( SIZE_P , sizeof(char));
	fileinfo[i].time = (char *) calloc( SIZE_T , sizeof(char));
	fileinfo[i].name = (char *) calloc( SIZE_N , sizeof(char));

	fileinfo[i].perm = getperm (fileinfo[i].perm, fileStat);
	fileinfo[i].name  = getname (fileinfo[i].name, path, fileinfo[i].perm[0], opt->flag_l);
	fileinfo[i].n_link = getlinks(fileStat);
	fileinfo[i].user = getuser (fileStat);
	fileinfo[i].group = getgroup(fileStat);
	fileinfo[i].f_size = getsize (fileStat);
	fileinfo[i].time = getdate (fileinfo[i].time , fileStat );
	fileinfo[i].tick = (long long int) fileStat.st_mtime;

	printdata(fileinfo , 1 , opt , col , -1);
	
	free(fileinfo[0].perm);
	free(fileinfo[0].time);
	free(fileinfo[0].name);
	free(fileinfo);	
	return 0;
}

char *getdate(char *final_time, struct stat fileStat) {
	int i, j, count;
	struct tm * info = localtime(&fileStat.st_mtime);
	char * time = asctime(info);
	i = j = count = 0;
	while (time[i++] != ' ');
	while (count != 2) {
		if (time[i] == ':')
			count++;
		if (count == 2) {
			break;
		}
		final_time[j++] = time[i++];
	}
	final_time[j] = '\0';	
	
	return final_time;
}

int getlinks(struct stat fileStat) {
	return fileStat.st_nlink;
}

int getsize(struct stat fileStat) {
	return fileStat.st_size;
}

char *getuser(struct stat fileStat) {
	struct passwd *pass = getpwuid(fileStat.st_uid);
	return pass->pw_name;
}

char * getgroup(struct stat fileStat) {
	struct group *pass = getgrgid(fileStat.st_gid);
	return pass->gr_name;
}

char *getname(char * name , char * n , char is_link , int flag_l) {
	char * temp = (char *) malloc(SIZE_N * sizeof(char));

	if (flag_l == 0 || is_link != 'l') {
		strcpy(name , n);
	} else {
		int r = readlink( n , temp , SIZE_N );
		if (r < 0) {
			perror("lstat");
			free(temp);
			strcpy(name , n);
			return name;
		}
		temp[r] = '\0';
		sprintf(name , "%s -> %s", n , temp);
	}
	free(temp);
	return name;
}

char * getperm(char * perm , struct stat fileStat) {
	if (S_ISLNK(fileStat.st_mode)) {
		perm[0] = 'l';
	} else if (S_ISDIR(fileStat.st_mode)) {
		perm[0] = 'd';
	} else if (S_ISCHR(fileStat.st_mode)) {
		perm[0] = 'c';
	} else if (S_ISSOCK(fileStat.st_mode)) {
		perm[0] = 's';
	} else if (S_ISFIFO(fileStat.st_mode)) {
		perm[0] = 'p';
	} else if (S_ISBLK(fileStat.st_mode)) {
		perm[0] = 'b';
	} else {
		perm[0] = '-';
	}
	perm[1] = ((fileStat.st_mode & S_IRUSR) ? 'r' : '-');
	perm[2] = ((fileStat.st_mode & S_IWUSR) ? 'w' : '-');
	perm[3] = ((fileStat.st_mode & S_IXUSR) ? 'x' : '-');
	perm[4] = ((fileStat.st_mode & S_IRGRP) ? 'r' : '-');
	perm[5] = ((fileStat.st_mode & S_IWGRP) ? 'w' : '-');
	perm[6] = ((fileStat.st_mode & S_IXGRP) ? 'x' : '-');
	perm[7] = ((fileStat.st_mode & S_IROTH) ? 'r' : '-');
	perm[8] = ((fileStat.st_mode & S_IWOTH) ? 'w' : '-');
	perm[9] = ((fileStat.st_mode & S_IXOTH) ? 'x' : '-');

	if (fileStat.st_mode & S_ISUID) {
		perm[3] = 's';
	} else if (fileStat.st_mode & S_IXUSR) {
		perm[3] = 'x';
	} else {
		perm[3] = '-';
	}

	if (fileStat.st_mode & S_ISGID) {
		perm[6] = 's';
	} else if (fileStat.st_mode & S_IXGRP) {
		perm[6] = 'x';
	} else {
		perm[6] = '-';
	}

	if (fileStat.st_mode & S_ISVTX) {
		perm[9] = 't';
	} else if (fileStat.st_mode & S_IXOTH) {
		perm[9] = 'x';
	} else {
		perm[9] = '-';
	}

	perm[10] = 0;

	return perm;
}


int main(int argc , char * argv[]) {
	SCRIPT_NAME = argv[0];
	char c = 0;
	char ** arg = NULL;
	int count_arg = 0;

	options_t * opt = (options_t *) calloc(1 , sizeof(options_t));

	for (int i=1; i < argc; i++) {
		if (strcmp(argv[i], "--help") == 0) {
			help_menu();
			return(0);
		} else if (strcmp(argv[i], "--version") == 0) {
			script_version();
			return(0);
		}
	}

	while ((c = getopt(argc, argv, "aSdltRhv")) != -1) {
		switch (c) {
			case 'a':
				opt->flag_a = 1;
		     		break;
			case 'S':
				opt->flag_S = 1;
				opt->flag_t = 0;
		     		break;
			case 'd':
				opt->flag_d = 1;
		     		break;
			case 'l':
				opt->flag_l = 1;
				break;
			case 't':
				opt->flag_t = 1;
				opt->flag_S = 0;
		     		break;
			case 'R':
				opt->flag_R = 1;
					break;
			case 'h':
				help_menu();
					break;
			case 'v':
				script_version();
					break;
			case '?':
			    	break;
			default:
			    	printf ("?? getopt returned character code 0%o ??\n", c);
		}
    }
    if (optind < argc) {
		arg = (char **) calloc((argc - optind), sizeof(char *));
		count_arg = 0;
		while (optind < argc) {
			arg[count_arg++] = argv[optind++];
		}
	}
	
	lf(opt , arg , count_arg);

	if (arg != NULL) {
		free(arg);
	}
	free(opt);

	return 0;

}

//
// End main-lf.c
//
