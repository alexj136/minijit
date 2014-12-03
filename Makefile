GCC     = gcc -Wall -g
SRCDIR  = ./src
BINDIR  = ./bin
MODULES = main util syntax lexer parser
MAIN    = ./main

all: $(MAIN)

$(MAIN): $(addprefix $(BINDIR)/, $(MODULES:=.o))
	$(GCC) -o $@ $^

.PHONY: ./src/main.h
$(BINDIR)/%.o: $(SRCDIR)/%.c $(SRCDIR)/%.h
	@mkdir -p ./bin/
	$(GCC) -c -o $@ $<

src/lexer.c: src/lexer.l
	flex -o $@ $^

src/parser.c: src/parser.y
	bison -o $@ $^

.PHONY: clean
clean:
	@rm -rf ./main ./bin/ ./src/lexer.c ./src/parser.c
