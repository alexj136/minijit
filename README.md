## MiniJIT

A mini jit compiler for a while language with functions

# Language Syntax

PROG ::= main(ARGS) { COMM } FUNC
FUNC ::= NAME(ARGS) { COMM } | ε
COMM ::= while EXPR do { COMM } | NAME := EXPR | COMM; COMM | return EXPR
EXPR ::= INT | EXPR + EXPR | EXPR - EXPR | EXPR * EXPR | NAME(ARGS)
ARGS ::= NAME ARGSCONT | ε
ARGSCONT ::= , NAME ARGSCONT | ε
