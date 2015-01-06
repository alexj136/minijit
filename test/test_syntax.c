#include "minunit.h"
#include "syntax.h"

MINUNIT_TESTS

    TEST("Test that two empty programs are equal")
        Prog *p = Prog_init(FuncVector_init());
        Prog *q = Prog_init(FuncVector_init());
        ASSERT(Prog_eq(p, q), "empty programs");
        Prog_free(p);
        Prog_free(q);
    END

    TEST("Test that an empty program is equal to itself")
        Prog *p = Prog_init(FuncVector_init());
        ASSERT(Prog_eq(p, p), "empty program with itself");
        Prog_free(p);
    END

    TEST("Non-equal programs")
        /*
         * P = 0() { return 10 }
         * Q = 0() { return 9 }
         */
        Func *f = Func_init(0, IntRefVector_init(), Return_init(Int_init(10)));
        FuncVector *fvp = FuncVector_init();
        FuncVector_append(fvp, f);
        Func *g = Func_init(0, IntRefVector_init(), Return_init(Int_init(9)));
        FuncVector *fvq = FuncVector_init();
        FuncVector_append(fvq, g);
        Prog *p = Prog_init(fvp);
        Prog *q = Prog_init(FuncVector_init());
        ASSERT(!Prog_eq(p, q), "empty programs");
        Prog_free(p);
        Prog_free(q);
    END

    TEST("Equality tests on various integer literal expressions")
        Expr *i, *j;

        i = Int_init(0);
        ASSERT(Expr_eq(i, i), "the same int literal");
        Expr_free(i);

        i = Int_init(0);
        j = Int_init(0);
        ASSERT(Expr_eq(i, j), "two zeroes");
        Expr_free(i);
        Expr_free(j);

        i = Int_init(0);
        j = Int_init(1);
        ASSERT(!Expr_eq(i, j), "zero and one");
        Expr_free(i);
        Expr_free(j);
    END

    TEST("Tests equality on some arithmetic expressions")
        Expr *i, *j;

        i = Add_init(Int_init(1), Int_init(2));
        j = Add_init(Int_init(1), Int_init(2));
        ASSERT(Expr_eq(i, j), "simple equal additions");
        Expr_free(i);
        Expr_free(j);

        i = Sub_init(Int_init(1), Int_init(2));
        j = Sub_init(Int_init(1), Int_init(2));
        ASSERT(Expr_eq(i, j), "simple equal subtractions");
        Expr_free(i);
        Expr_free(j);

        i = Add_init(Int_init(1), Int_init(2));
        j = Add_init(Int_init(2), Int_init(2));
        ASSERT(!Expr_eq(i, j), "simple different additions");
        Expr_free(i);
        Expr_free(j);

        i = Sub_init(Int_init(1), Int_init(2));
        j = Sub_init(Int_init(2), Int_init(2));
        ASSERT(!Expr_eq(i, j), "simple different subtractions");
        Expr_free(i);
        Expr_free(j);

        i = Sub_init(Int_init(1), Int_init(2));
        j = Add_init(Int_init(1), Int_init(2));
        ASSERT(!Expr_eq(i, j), "subtraction vs addition");
        Expr_free(i);
        Expr_free(j);
    END

    TEST("Simple function call tests")
        Expr *e1, *e2;
        ExprVector *args1, *args2;

        args1 = ExprVector_init();
        ExprVector_append(args1, Int_init(1));
        e1 = Call_init(0, args1);
        args2 = ExprVector_init();
        ExprVector_append(args2, Int_init(1));
        e2 = Call_init(0, args2);
        ASSERT(Expr_eq(e1, e1), "the same simple function call");
        ASSERT(Expr_eq(e2, e2), "the same simple function call");
        ASSERT(Expr_eq(e1, e2), "simple identical function calls");
        Expr_free(e1);
        Expr_free(e2);

        args1 = ExprVector_init();
        ExprVector_append(args1, Int_init(1));
        e1 = Call_init(0, args1);
        args2 = ExprVector_init();
        ExprVector_append(args2, Int_init(2));
        e2 = Call_init(0, args2);
        ASSERT(!Expr_eq(e1, e2), "simple non-equal function calls");
        Expr_free(e1);
        Expr_free(e2);

        args1 = ExprVector_init();
        ExprVector_append(args1, Add_init(Int_init(1), Var_init(1)));
        e1 = Call_init(0, args1);
        args2 = ExprVector_init();
        ExprVector_append(args2, Add_init(Var_init(1), Int_init(1)));
        e2 = Call_init(0, args2);
        ASSERT(!Expr_eq(e1, e2), "more interesting non-equal function calls");
        Expr_free(e1);
        Expr_free(e2);
    END

    TEST("Variables test")
        Expr *i, *j;

        i = Var_init(0);
        ASSERT(Expr_eq(i, i), "the same variable occurrence");
        Expr_free(i);

        i = Var_init(0);
        j = Var_init(0);
        ASSERT(Expr_eq(i, j), "the same variable (different occurrence)");
        Expr_free(i);
        Expr_free(j);

        i = Var_init(0);
        j = Var_init(1);
        ASSERT(!Expr_eq(i, j), "different variables");
        Expr_free(i);
        Expr_free(j);
    END

    TEST("Variables not equal to int literals")
        Expr *i, *j;

        i = Int_init(0);
        j = Var_init(0);
        ASSERT(!Expr_eq(i, j), "Variable not equal to int regardless of value");
        Expr_free(i);
        Expr_free(j);
    END

END_TESTS
