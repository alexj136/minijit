#include "minunit.h"

DECLARE_MINUNIT_MODULE

INITIAL_SETUP
END_INITIAL_SETUP

SETUP
END_SETUP

BEGIN_TESTS
    TEST("Assert true")
        ASSERT(true, "is true true?");
    END
    TEST("Assert false")
        ASSERT(false, "is false true?");
    END
END_TESTS

TEARDOWN
END_TEARDOWN

FINAL_TEARDOWN
END_FINAL_TEARDOWN
