#include "minunit.h"
#include "syntax.h"
#include "interpreter.h"

MINUNIT_TESTS

    TEST("Store creation")

        // Create an IntRefVector of names
        IntRefVector *names = IntRefVector_init();
        int name = 0;
        while(name < 10) {
            IntRefVector_append(names, IntRef_init(name));
            name++;
        }

        // Create an IntRefVector of values
        IntRefVector *values = IntRefVector_init();
        int val = 10;
        while(val > 0) {
            IntRefVector_append(values, IntRef_init(val));
            val--;
        }

        // Create the store
        Store *store = Store_init(names, values);

        // Test that the mapping remains correct
        name = 9;
        val = 1;
        while(val <= 10) {
            IntRef *name_to_lookup = IntRef_init(name);

            ASSERT(Store_lookup(store, name_to_lookup) == val,
                    "Store did not map to correct value");

            name--;
            val++;
            IntRef_free(name_to_lookup);
        }

        // Free stuff
        IntRefVector_free_elems(names);
        Store_free(store);
    END

END_TESTS
