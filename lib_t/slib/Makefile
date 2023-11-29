SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))

ALL: test

test: $(OBJ)
	gcc $(OBJ) -o $@ -std=c99

$(OBJ): $(SRC)
	gcc -c $^ -o $(OBJ)

clean:
	rm -rf test $(OBJ) 

.PHONY: clean ALL
