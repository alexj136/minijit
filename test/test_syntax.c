#include "minunit.h"
#include "syntax.h"

MINUNIT_TESTS

    TEST("Test that two empty programs are equal")
        Prog *p = Prog_init(FuncVector_init(), charVector_init());
        Prog *q = Prog_init(FuncVector_init(), charVector_init());
        ASSERT(Prog_eq(p, q), "empty programs");
        Prog_free(p);
        Prog_free(q);
    END

    TEST("Test that an empty program is equal to itself")
        Prog *p = Prog_init(FuncVector_init(), charVector_init());
        ASSERT(Prog_eq(p, p), "empty program with itself");
        Prog_free(p);
    END

    TEST("Simple non-equal programs")
        /*
         * P = 0() { return 10 }
         * Q = 0() { return 9 }
         */
        Func *f = Func_init(0, 0, IntRefVector_init(),
                Return_init(Int_init(10)));
        FuncVector *fvp = FuncVector_init();
        FuncVector_append(fvp, f);
        Func *g = Func_init(0, 0, IntRefVector_init(),
                Return_init(Int_init(9)));
        FuncVector *fvq = FuncVector_init();
        FuncVector_append(fvq, g);
        Prog *p = Prog_init(fvp, charVector_init());
        Prog *q = Prog_init(fvq, charVector_init());
        ASSERT(!Prog_eq(p, q), "Non-equal programs");
        Prog_free(p);
        Prog_free(q);
    END

    TEST("Simple equal programs")
        /*
         * P = 0() { return 0() }
         * Q = 0() { return 0() }
         */
        Comm *e1 = Return_init(Call_init(0, ExprVector_init()));
        Func *f = Func_init(0, 0, IntRefVector_init(), e1);
        FuncVector *fvp = FuncVector_init();
        FuncVector_append(fvp, f);
        Comm *e2 = Return_init(Call_init(0, ExprVector_init()));
        Func *g = Func_init(0, 0, IntRefVector_init(), e2);
        FuncVector *fvq = FuncVector_init();
        FuncVector_append(fvq, g);
        Prog *p = Prog_init(fvp, charVector_init());
        Prog *q = Prog_init(fvq, charVector_init());
        ASSERT(Prog_eq(p, q), "Equal programs");
        Prog_free(p);
        Prog_free(q);
    END

    TEST("Simple equal functions")
        Comm *e1 = Return_init(Call_init(0, ExprVector_init()));
        Func *f = Func_init(0, 0, IntRefVector_init(), e1);
        FuncVector *fvp = FuncVector_init();
        FuncVector_append(fvp, f);
        Comm *e2 = Return_init(Call_init(0, ExprVector_init()));
        Func *g = Func_init(0, 0, IntRefVector_init(), e2);
        FuncVector *fvq = FuncVector_init();
        FuncVector_append(fvq, g);
        ASSERT(Func_eq(f, g), "Equal functions");
        FuncVector_free_elems(fvp);
        FuncVector_free_elems(fvq);
    END

    TEST("Simple non-equal functions")
        Comm *e1 = Return_init(Call_init(1, ExprVector_init()));
        Func *f = Func_init(0, 0, IntRefVector_init(), e1);
        FuncVector *fvp = FuncVector_init();
        FuncVector_append(fvp, f);
        Comm *e2 = Return_init(Call_init(0, ExprVector_init()));
        Func *g = Func_init(0, 0, IntRefVector_init(), e2);
        FuncVector *fvq = FuncVector_init();
        FuncVector_append(fvq, g);
        ASSERT(!Func_eq(f, g), "Non-equal functions");
        FuncVector_free_elems(fvp);
        FuncVector_free_elems(fvq);
    END

    TEST("Simple while command equality")
        Comm *c1, *c2;

        c1 = While_init(Int_init(1), Return_init(Int_init(1)));
        c2 = While_init(Int_init(1), Return_init(Int_init(1)));
        ASSERT(Comm_eq(c1, c2), "Equal while commands");
        Comm_free(c1);
        Comm_free(c2);

        c1 = While_init(Int_init(0), Return_init(Int_init(1)));
        c2 = While_init(Int_init(1), Return_init(Int_init(1)));
        ASSERT(!Comm_eq(c1, c2), "Different guards but equal bodies");
        Comm_free(c1);
        Comm_free(c2);

        c1 = While_init(Int_init(1), Return_init(Int_init(0)));
        c2 = While_init(Int_init(1), Return_init(Int_init(1)));
        ASSERT(!Comm_eq(c1, c2), "Different bodies but equal guards");
        Comm_free(c1);
        Comm_free(c2);

        c1 = While_init(Int_init(1), Return_init(Int_init(0)));
        c2 = While_init(Int_init(0), Return_init(Int_init(1)));
        ASSERT(!Comm_eq(c1, c2), "Different bodies and guards");
        Comm_free(c1);
        Comm_free(c2);
    END

    TEST("Simple return command equality test")
        Comm *c1, *c2;

        c1 = Return_init(Call_init(0, ExprVector_init()));
        c2 = Return_init(Call_init(0, ExprVector_init()));
        ASSERT(Comm_eq(c1, c2), "Equal return commands");
        Comm_free(c1);
        Comm_free(c2);

        c1 = Return_init(Call_init(1, ExprVector_init()));
        c2 = Return_init(Call_init(0, ExprVector_init()));
        ASSERT(!Comm_eq(c1, c2), "Non-equal return commands");
        Comm_free(c1);
        Comm_free(c2);
    END

    TEST("Simple assignment command equality")
        Comm *c1, *c2;

        c1 = Assign_init(1, Int_init(1));
        c2 = Assign_init(1, Int_init(1));
        ASSERT(Comm_eq(c1, c2), "Equal assignment commands");
        Comm_free(c1);
        Comm_free(c2);

        c1 = Assign_init(1, Int_init(1));
        c2 = Assign_init(1, Int_init(0));
        ASSERT(!Comm_eq(c1, c2), "Same variable but different expression");
        Comm_free(c1);
        Comm_free(c2);

        c1 = Assign_init(1, Int_init(1));
        c2 = Assign_init(0, Int_init(1));
        ASSERT(!Comm_eq(c1, c2), "Different variable but same expression");
        Comm_free(c1);
        Comm_free(c2);

        c1 = Assign_init(1, Int_init(1));
        c2 = Assign_init(0, Int_init(0));
        ASSERT(!Comm_eq(c1, c2), "Different expressions and variable");
        Comm_free(c1);
        Comm_free(c2);
    END

    TEST("Simple composition command equality")
        Comm *c1, *c2;

        c1 = Comp_init(Return_init(Int_init(1)), Return_init(Int_init(1)));
        c2 = Comp_init(Return_init(Int_init(1)), Return_init(Int_init(1)));
        ASSERT(Comm_eq(c1, c2), "Equal composition commands");
        Comm_free(c1);
        Comm_free(c2);

        c1 = Comp_init(Return_init(Int_init(1)), Return_init(Int_init(1)));
        c2 = Comp_init(Return_init(Int_init(1)), Return_init(Int_init(0)));
        ASSERT(!Comm_eq(c1, c2), "Same first but different second commands");
        Comm_free(c1);
        Comm_free(c2);

        c1 = Comp_init(Return_init(Int_init(1)), Return_init(Int_init(1)));
        c2 = Comp_init(Return_init(Int_init(0)), Return_init(Int_init(1)));
        ASSERT(!Comm_eq(c1, c2), "Same second but different first commands");
        Comm_free(c1);
        Comm_free(c2);

        c1 = Comp_init(Return_init(Int_init(1)), Return_init(Int_init(0)));
        c2 = Comp_init(Return_init(Int_init(0)), Return_init(Int_init(1)));
        ASSERT(!Comm_eq(c1, c2), "Different first and second commands");
        Comm_free(c1);
        Comm_free(c2);
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
