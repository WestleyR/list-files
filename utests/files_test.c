#include <stdlib.h>
#include "utest.h"
#include "../src/bool.h"

#include "../src/files.h"

UTEST_MAIN();

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

// vim: tabstop=2 shiftwidth=2 expandtab autoindent softtabstop=0
