GCC     = gcc -Wall -Wextra -g -rdynamic
SRCDIR  = ./src
BINDIR  = ./bin
TESTDIR = ./test
MODULES = util syntax lexer parser typecheck interpreter icode
MAIN    = ./main

all: $(MAIN)

$(MAIN): $(addprefix $(BINDIR)/, $(MODULES:=.o)) ./src/main.c
	$(GCC) -o $@ $^

$(BINDIR)/%.o: $(SRCDIR)/%.c $(SRCDIR)/%.h src/lexer.c src/parser.c
	@mkdir -p ./bin/
	$(GCC) -c -o $@ $<

src/lexer.c: src/lexer.l
	flex -o $@ $^

src/parser.c: src/parser.y
	bison -o $@ $^

.PHONY: clean
clean:
	@rm -rf ./main ./bin/ ./src/lexer.c ./src/parser.c

test: $(addprefix $(BINDIR)/test_, $(MODULES))
	@$(addprefix $(BINDIR)/test_, $(MODULES:= && )) echo "All tests passed."

$(BINDIR)/test_%: $(TESTDIR)/test_%.c $(addprefix $(BINDIR)/, $(MODULES:=.o))
	$(GCC) -o $@ $^ -iquote $(SRCDIR)
