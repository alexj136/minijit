#include "minunit.h"
#include "util.h"

DECLARE_MINUNIT_MODULE

INITIAL_SETUP
END_INITIAL_SETUP

SETUP
END_SETUP

BEGIN_TESTS
    TEST("Tests of str_equal()")
        ASSERT(str_equal("", ""), "empty strings");
        ASSERT(str_equal("aaa", "aaa"), "simple strings");
        ASSERT(str_equal(NULL, NULL), "two null strings");
        ASSERT(!str_equal("foo", NULL), "right null string");
        ASSERT(!str_equal(NULL, "bar"), "left null string");
        ASSERT(!str_equal("abc", "cba"), "different strings");
    END
END_TESTS

TEARDOWN
END_TEARDOWN

FINAL_TEARDOWN
END_FINAL_TEARDOWN
