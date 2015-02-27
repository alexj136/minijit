#include "minunit.h"
#include "syntax.h"
#include "interpreter.h"

MINUNIT_TESTS

    TEST("Variables evaluate correctly")

        int store[] = { 1, 1, 2, 3, 5 };

        Expr *v0 = Var_init(0);
        Expr *v1 = Var_init(1);
        Expr *v2 = Var_init(2);
        Expr *v3 = Var_init(3);
        Expr *v4 = Var_init(4);

        InterpretResult *ir0 = interpret_Expr(NULL, v0, store);
        InterpretResult *ir1 = interpret_Expr(NULL, v1, store);
        InterpretResult *ir2 = interpret_Expr(NULL, v2, store);
        InterpretResult *ir3 = interpret_Expr(NULL, v3, store);
        InterpretResult *ir4 = interpret_Expr(NULL, v4, store);

        ASSERT(ir0->type == iSuccess && ir0->result == 1,
                "interpret_Expr retrieves variable correctly");
        ASSERT(ir1->type == iSuccess && ir1->result == 1,
                "interpret_Expr retrieves variable correctly");
        ASSERT(ir2->type == iSuccess && ir2->result == 2,
                "interpret_Expr retrieves variable correctly");
        ASSERT(ir3->type == iSuccess && ir3->result == 3,
                "interpret_Expr retrieves variable correctly");
        ASSERT(ir4->type == iSuccess && ir4->result == 5,
                "interpret_Expr retrieves variable correctly");

        Expr_free(v0);
        Expr_free(v1);
        Expr_free(v2);
        Expr_free(v3);
        Expr_free(v4);

        free(ir0);
        free(ir1);
        free(ir2);
        free(ir3);
        free(ir4);

    END

    TEST("Randomised addition and subtraction of literals")

        int idx;
        for(idx = 0; idx < 5000; idx++) {

            int lhs = (rand() % 10000) - 5000;
            int rhs = (rand() % 10000) - 5000;
            bool subtract = rand() % 2;
            Expr *expr;

            if(subtract) {
                expr = Sub_init(Int_init(lhs), Int_init(rhs));
            }
            else {
                expr = Add_init(Int_init(lhs), Int_init(rhs));
            }

            InterpretResult *res = interpret_Expr(NULL, expr, NULL);

            ASSERT(res->result == (subtract ? lhs - rhs : lhs + rhs),
                    "Arithmetic evaluates correctly")

            Expr_free(expr);
            free(res);
        }

    END

END_TESTS
