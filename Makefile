GCC     = gcc -Wall -g
SRCDIR  = ./src
BINDIR  = ./bin
MODULES = main util syntax lexer
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

.PHONY: clean
clean:
	@rm -rf ./main ./bin/ ./src/lexer.c
