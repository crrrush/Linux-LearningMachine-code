SRC = $(wildcard *.c)
#OBJ = $(patsubst %.c, %.o, $(SRC))

ALL: test

test: $(SRC)
	gcc $^ -o $@ -std=c99

#$(OBJ): $(SRC)
#	gcc -c $^ -o $@

clean:
	rm -rf  test 

.PHONY: clean ALL
