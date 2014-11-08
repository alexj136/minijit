## MiniJIT

A mini jit compiler for a while language, written in C, targetting x86\_64
GNU/Linux.

# Language Syntax

- PROG ::= main(ARGS) { COMM } FUNC
- FUNC ::= NAME(ARGS) { COMM } | ε
- COMM ::= while EXPR do { COMM } | NAME := EXPR | COMM; COMM | return EXPR
- EXPR ::= INT | EXPR + EXPR | EXPR - EXPR | EXPR * EXPR | NAME(ARGS) | NAME
- ARGS ::= NAME ARGSCONT | ε
- ARGSCONT ::= , NAME ARGSCONT | ε
- NAME ::= [a-zA-Z_][a-zA-Z0-9_]*
