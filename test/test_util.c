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

    TEST("Test of Vectors")
        charVector *cv = charVector_init();
        ASSERT(charVector_size(cv) == 0, "Length 1");
        charVector_append(cv, "a");
        ASSERT(charVector_size(cv) == 1, "Length 2");
        charVector_append(cv, "b");
        ASSERT(charVector_size(cv) == 2, "Length 3");
        charVector_remove(cv, 0);
        ASSERT(charVector_size(cv) == 1, "Length 4");
        ASSERT(str_equal(charVector_get(cv, 0), "b"), "Elems shift down");
        charVector_remove(cv, 0);
        ASSERT(charVector_size(cv) == 0, "Length 5");
        charVector_free(cv);
    END

    TEST("Another test of Vectors")
        charVector *cv = charVector_init();

        int idx;
        for(idx = 0; idx < 100; idx++) {
            ASSERT(charVector_size(cv) == idx, "Length match 1");
            charVector_append(cv, "a");
        }

        charVector_append(cv, "b");

        for(idx = 100; idx > 0; idx--) {
            ASSERT(charVector_size(cv) == idx + 1, "Length match 2");
            ASSERT(str_equal(charVector_remove(cv, 0), "a"), "Content match 1");
        }

        ASSERT(str_equal(charVector_remove(cv, 0), "b"), "Content match 2");

        charVector_free(cv);
    END

END_TESTS

TEARDOWN
END_TEARDOWN

FINAL_TEARDOWN
END_FINAL_TEARDOWN
