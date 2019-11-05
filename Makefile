EXE = myshell

SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=obj/%.o)

CFLAGS += -Wall -pedantic -g

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $^ -o $@ 

obj/%.o: src/%.c
	$(CC) -Iinclude $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) $(EXE) test
