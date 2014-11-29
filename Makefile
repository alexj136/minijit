GCC     = gcc -Wall -g
SRCDIR  = ./src
BINDIR  = ./bin
MODULES = main util syntax
MAIN    = ./main

all: $(MAIN)

clean:
	@rm -rf ./bin/
	@rm -f ./main

$(MAIN): $(addprefix $(BINDIR)/, $(MODULES:=.o))
	$(GCC) -o $@ $^

$(BINDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p ./bin/
	$(GCC) -c -o $@ $<
