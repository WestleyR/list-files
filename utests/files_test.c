#include <stdlib.h>
#include <string.h>
#include "utest.h"
#include "../src/bool.h"

#include "../src/files.h"

UTEST_MAIN();

// return most be freed.
char* octal_int_to_str(int octal) {
  char ret[12];

  sprintf(ret, "%04o", octal);

  return strdup(ret);
}

UTEST(files, get_octal_perm) {
  struct test_case {
    char input[256];
    char out[12];
  };

  struct test_case tests[] = {
    {
      "tests/testdata/file1",
      "0664",
    },
  };

  for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
    int got = get_file_octal_perm(tests[i].input);

    char* gotstr = octal_int_to_str(got);

    EXPECT_STREQ(tests[i].out, gotstr);
    free(gotstr);
  }
}

UTEST(files, human_readable_fs) {
  struct test_case {
    char expected[80];
    double input;
  };

  struct test_case tests[] = {
    {
      "1000 B",
      1000,
    },
    {
      "1023 B",
      1023,
    },
    {
      "1024 B",
      1024,
    },
    {
      "1.0 KB",
      1025,
    },
    {
      "1.1 KB",
      1123,
    },
    {
      "37.39 MB",
      39209482,
    },
  };

  for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
    char* got = human_readable_bytes(tests[i].input);

    EXPECT_STREQ(tests[i].expected, got);
    free(got);
  }
}

UTEST(files, find_link) {
  struct test_case {
    char input[256];
    char expected[256];
  };

  struct test_case tests[] = {
    {
      "tests/testdata/link_to",
      "link_from",
    },
    {
      "tests/testdata/broken-link",
      "non-existent-link",
    },
    {
      "tests/testdata/file1",
      "",
    },
  };

  for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
    char* got = find_link(tests[i].input);

    if (tests[i].expected[0] == '\0') {
      EXPECT_TRUE(got == NULL);
    } else {
      EXPECT_STREQ(tests[i].expected, got);
      free(got);
    }
  }
}


UTEST(file, get_filedate) {
  struct test_case {
    char input[256];
    char expected[256];
  };

  struct test_case tests[] = {
    {
      "tests/testdata/link_to",
      "202", // for 2021-05-4
    },
  };

  for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
    struct stat info;

    if (lstat(tests[i].input, &info) != 0) {
      printf("error: unable to open stat on: %s\n", tests[i].input);
      ASSERT_TRUE(false);
    }

    char* date = get_filedate(info);
    EXPECT_TRUE(date != NULL);

    EXPECT_STRNEQ(tests[i].expected, date, strlen(tests[i].expected));
    free(date);
  }
}

// vim: tabstop=2 shiftwidth=2 expandtab autoindent softtabstop=0
