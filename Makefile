GCC    = gcc -Wall -g
SRCDIR = ./src
BINDIR = ./bin
MAIN   = ./main
MODULES= main util syntax
SOURCES= $(MODULES:./src/%.c)
OBJECTS= $(MODULES:./bin/%.o)

all: $(MAIN)

clean:
	@rm -rf ./bin/

$(MAIN): ./bin/main.o ./bin/util.o ./bin/syntax.o
	@mkdir -p ./bin/
	$(GCC) -o $(MAIN) $(BINDIR)/*.o

$(BINDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p ./bin/
	$(GCC) -c -o $(output) $(input)

#./bin/main.o: ./src/main.c
#	@mkdir -p ./bin/
#	$(GCC) -c -o ./bin/main.o ./src/main.c
#
#./bin/util.o: ./src/util.c
#	@mkdir -p ./bin/
#	$(GCC) -c -o ./bin/util.o ./src/util.c
#
#./bin/syntax.o: ./src/syntax.c
#	@mkdir -p ./bin/
#	$(GCC) -c -o ./bin/syntax.o ./src/syntax.c
