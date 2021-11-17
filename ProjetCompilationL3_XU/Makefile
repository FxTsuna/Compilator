CC=gcc
OBJ=abstract-tree.o table-des-symboles.o lex.yy.o
CFLAGS=-Wall -pedantic -g
LDFLAGS=-Wall -pedantic -ll -g
EXEC=tpcc

all : $(EXEC)

$(EXEC): $(EXEC).tab.c $(OBJ) 
	$(CC) src/$(EXEC).tab.c $(OBJ:%.o=obj/%.o) -o bin/$(EXEC)

lex.yy.o: lex.yy.c
	$(CC) -c src/lex.yy.c -o obj/lex.yy.o $(CFLAGS)

lex.yy.c: src/$(EXEC).lex
	flex -o src/$@ src/$(EXEC).lex

%.o: src/%.c src/%.h
	$(CC) -c $< -o obj/$@ $(CFLAGS)

$(EXEC).tab.c: src/$(EXEC).y
	bison -d -v src/$(EXEC).y
	@mv $(EXEC).tab.c src/
	@mv $(EXEC).tab.h src/

clean:
	rm -f src/lex.yy.* src/$(EXEC).tab.* $(EXEC).output obj/*