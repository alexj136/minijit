#ifndef testing_programs
#define testing_programs

char *testing_programs_array[] = {

    // Super simple program that just returns an integer literal
    "main() { return 10 }",

    // Super simple program that just returns its single argument
    "main(a) { return a }",

    // Simple program that just returns the sum of its two arguments
    "main(a, b) { return a + b }",

    // Simple program that just returns the subtraction of its second argument
    // from the first
    "main(a, b) { return a - b }",

    // Simple program that reassigns its argument to be one greater than its
    // initial value, and then returns it
    "main(a) { a := a + 1; return a }",

    // Program that computes the product of its arguments, given that they are
    // both greater than or equal to 1
    "  main(a, b) {            "
    "      c := a;             "
    "      while b - 1 do {    "
    "          b := b - 1;     "
    "          a := a + c      "
    "      };                  "
    "      return a            "
    "  }                       ",

    // Program that does a simple function call
    "  main(a) {               "
    "      return foo(a) + a   "
    "  }                       "
    "  foo(a) {                "
    "      return a + 10       "
    "  }                       "
};

#endif // testing_programs
